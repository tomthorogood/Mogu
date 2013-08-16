#include <iostream>
#include "../../src/Security/UserManager.h"
#include "../../src/Redis/DatabaseConfigReader.h"
#include "../../src/Redis/QueryHandler/Context.h"
#include <cpptest.h>
#include <hiredis/hiredis.h>

namespace Application {
    extern Context_Map* context_map;
}

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
        Redis::Context* cxt = Application::context_map->get(Prefix::user);
        redisContext* rdb = redisConnect(cxt->host.c_str(),cxt->port);
        redisReply* r = (redisReply*) redisCommand(rdb, "select %d", cxt->db_num);
        freeReplyObject(r);
        r = (redisReply*) redisCommand(rdb,"flushdb");
        freeReplyObject(r);
        redisFree(rdb);
    }
};

std::string strStatus(Security_Status s)
{
    switch(s)
    {
        case Security_Status::ok_login: return "ok_login";
        case Security_Status::ok_register: return "ok_register";
        case Security_Status::err_user_exists: return "err_user_exists";
        case Security_Status::err_bad_auth:  return "err_bad_auth";
        case Security_Status::err_user_not_found: return "err_user_not_found";
        case Security_Status::ok_password_reset: return "ok_password_reset";
        default: return "ERR_UNKNOWN";
    }
}

void UserManagerTestSuite::register_new_user()
{
    User_Manager m;
    Security_Status status = m.register_user(good_username, good_password);
    std::string str_status = strStatus(status);
    TEST_ASSERT_MSG(status==Security_Status::ok_register, str_status.c_str());
    assigned_id = m.get_user();
}

void UserManagerTestSuite::register_existing_user()
{
    User_Manager m;
    Security_Status status = m.register_user(good_username, bad_password);
    std::string str_status = strStatus(status);
    TEST_ASSERT_MSG(status==Security_Status::err_user_exists, str_status.c_str());
}

void UserManagerTestSuite::login_existing_success()
{
    User_Manager m;
    Security_Status status = m.login_user(good_username, good_password);
    std::string str_status = strStatus(status);
    TEST_ASSERT_MSG(status==Security_Status::ok_login, str_status.c_str());
}

void UserManagerTestSuite::login_nonexisting_user()
{
    User_Manager m;
    Security_Status status = m.login_user(bad_username, good_password);
    std::string str_status = strStatus(status);
    TEST_ASSERT_MSG(status==Security_Status::err_user_not_found, str_status.c_str());
}

void UserManagerTestSuite::login_existing_fail()
{
    User_Manager m;
    Security_Status status = m.login_user(good_username, bad_password);
    std::string str_status = strStatus(status);
    TEST_ASSERT_MSG(status==Security_Status::err_bad_auth, str_status.c_str());
}

void UserManagerTestSuite::login_existing_CAPS_success()
{
    User_Manager m;
    Security_Status status = m.login_user(caps_username, good_password);
    std::string str_status = strStatus(status);
    TEST_ASSERT_MSG(status==Security_Status::ok_login, str_status.c_str());
}

void UserManagerTestSuite::login_existing_CAPS_fail()
{
    User_Manager m;
    Security_Status status = m.login_user(caps_username, caps_password);
    std::string str_status = strStatus(status);
    TEST_ASSERT_MSG(status==Security_Status::err_bad_auth, str_status.c_str());
}

int main()
{
    Application::load_database_contexts();
    Test::TextOutput output(Test::TextOutput::Verbose);
    UserManagerTestSuite umts;
    return umts.run(output,false) ? EXIT_SUCCESS : EXIT_FAILURE;
}
