#include <iostream>
#include "../../src/Security/UserManager.h"
#include "../../src/Redis/DatabaseConfigReader.h"
#include "../../src/Redis/QueryHandler/Context.h"
#include <cpptest.h>
#include <hiredis/hiredis.h>

class UserManagerTestSuite : public Test::Suite
{
public:
    UserManagerTestSuite()
    {
        flushdb();

        TEST_ADD(UserManagerTestSuite::register_new_user);
        TEST_ADD(UserManagerTestSuite::register_existing_user);
        TEST_ADD(UserManagerTestSuite::login_existing_success);
        TEST_ADD(UserManagerTestSuite::login_nonexisting_user);
        TEST_ADD(UserManagerTestSuite::login_existing_fail);
        //Make sure case sensitivity is not bound to usernames
        TEST_ADD(UserManagerTestSuite::login_existing_CAPS_success);
        //Make sure case sensitivity is bound to passwords
        TEST_ADD(UserManagerTestSuite::login_existing_CAPS_fail);
    }

    ~UserManagerTestSuite() { flushdb(); }

private:
    void register_new_user();
    void register_existing_user();
    void login_existing_success();
    void login_nonexisting_user();
    void login_existing_fail();
    void login_existing_CAPS_success();
    void login_existing_CAPS_fail();

    std::string good_username = "test@domain.com";
    std::string good_password = "passw0rd";
    std::string bad_username = "t3st@domain.com";
    std::string bad_password = "p@55w0rd";
    std::string caps_username = "TEST@DOMAIN.COM";
    std::string caps_password = "PASSW0RD";

    int assigned_id = -1;

    void flushdb()
    {
        Redis::Context* cxt = Application::contextMap->get(Prefix::user);
        redisContext* rdb = redisConnect(cxt->host.c_str(),cxt->port);
        redisReply* r = (redisReply*) redisCommand(rdb, "select %d", cxt->db_num);
        freeReplyObject(r);
        r = (redisReply*) redisCommand(rdb,"flushdb");
        freeReplyObject(r);
        redisFree(rdb);
    }
};

std::string strStatus(SecurityStatus s)
{
    switch(s)
    {
        case SecurityStatus::OK_LOGIN: return "OK_LOGIN";
        case SecurityStatus::OK_REGISTER: return "OK_REGISTER";
        case SecurityStatus::ERR_USER_EXISTS: return "ERR_USER_EXISTS";
        case SecurityStatus::ERR_BAD_AUTH:  return "ERR_BAD_AUTH";
        case SecurityStatus::ERR_USER_NOT_FOUND: return "ERR_USER_NOT_FOUND";
        case SecurityStatus::OK_PASSWORD_RESET: return "OK_PASSWORD_RESET";
        default: return "ERR_UNKNOWN";
    }
}

void UserManagerTestSuite::register_new_user()
{
    UserManager m;
    SecurityStatus status = m.registerUser(good_username, good_password);
    std::string str_status = strStatus(status);
    TEST_ASSERT_MSG(status==SecurityStatus::OK_REGISTER, str_status.c_str());
    assigned_id = m.getUser();
}

void UserManagerTestSuite::register_existing_user()
{
    UserManager m;
    SecurityStatus status = m.registerUser(good_username, bad_password);
    std::string str_status = strStatus(status);
    TEST_ASSERT_MSG(status==SecurityStatus::ERR_USER_EXISTS, str_status.c_str());
}

void UserManagerTestSuite::login_existing_success()
{
    UserManager m;
    SecurityStatus status = m.loginUser(good_username, good_password);
    std::string str_status = strStatus(status);
    TEST_ASSERT_MSG(status==SecurityStatus::OK_LOGIN, str_status.c_str());
}

void UserManagerTestSuite::login_nonexisting_user()
{
    UserManager m;
    SecurityStatus status = m.loginUser(bad_username, good_password);
    std::string str_status = strStatus(status);
    TEST_ASSERT_MSG(status==SecurityStatus::ERR_USER_NOT_FOUND, str_status.c_str());
}

void UserManagerTestSuite::login_existing_fail()
{
    UserManager m;
    SecurityStatus status = m.loginUser(good_username, bad_password);
    std::string str_status = strStatus(status);
    TEST_ASSERT_MSG(status==SecurityStatus::ERR_BAD_AUTH, str_status.c_str());
}

void UserManagerTestSuite::login_existing_CAPS_success()
{
    UserManager m;
    SecurityStatus status = m.loginUser(caps_username, good_password);
    std::string str_status = strStatus(status);
    TEST_ASSERT_MSG(status==SecurityStatus::OK_LOGIN, str_status.c_str());
}

void UserManagerTestSuite::login_existing_CAPS_fail()
{
    UserManager m;
    SecurityStatus status = m.loginUser(caps_username, caps_password);
    std::string str_status = strStatus(status);
    TEST_ASSERT_MSG(status==SecurityStatus::ERR_BAD_AUTH, str_status.c_str());
}

int main()
{
    Application::loadDatabaseContexts();
    Test::TextOutput output(Test::TextOutput::Verbose);
    UserManagerTestSuite umts;
    return umts.run(output,false) ? EXIT_SUCCESS : EXIT_FAILURE;
}
