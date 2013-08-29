#include <cpptest.h>
#include <src/Types/GroupManager.h>
#include <src/Types/NodeValue.h>
#include <src/Security/UserManager.h>
#include <src/Types/Prefixes.h>
#include <src/Redis/NodeEditor.h>

#include <vector>
#include <string>
#include <map>

const std::map <Security_Status, const char*> status_strings
{
    {Security_Status::err_unknown, "Error: Unknown Error"},
    {Security_Status::err_user_exists, "Error: User exists in context"},
    {Security_Status::err_bad_auth, "Error: Bad authorization"},
    {Security_Status::err_no_group_selected, "Error: Performing operation on unselected group"},
    {Security_Status::ok_register, "Notice: Registration Success"},
    {Security_Status::ok_login, "Notice: Login success"},
    {Security_Status::ok_password_reset, "Notice:: Password reset successfully"},
    {Security_Status::ok_remove, "Notice: Removal succeeded in context"}
};


std::string cmpstr(const std::string& a, const std::string& b)
{
    std::stringstream buf;
    buf << "A: " << a << ", B: " << b;
    return buf.str();
}

class GMTS : public Test::Suite
{
public:
    GMTS() {
        TEST_ADD(GMTS::basic_constructors);
        TEST_ADD(GMTS::create_group);
        TEST_ADD(GMTS::key_resolution);
        TEST_ADD(GMTS::add_users);
        TEST_ADD(GMTS::user_table);
        TEST_ADD(GMTS::remove_users);
    }
private:
    std::string group_key {};
    int group_id {};

    void basic_constructors();
    void create_group();
    void key_resolution();
    void add_users();
    void remove_users();
    void user_table();
};

void populate_users()
{
    Node_Value arg1 {"first"};
    Node_Value arg2 {"last"};

    Redis::Node_Editor e {Prefix::user, "name"};
    std::vector <std::pair<std::string,std::string>> names {
        { "Tom" , "Thorogood" },
        { "Ben" , "Jaworowski" },
        { "Zoe" , "Irwin" },
        { "Whitney", "Scarberry" },
        { "Dan" , "Lucksinger" }
    };

    for (auto u : names)
    {
        User_Manager m {};

        //////////////////////////////////////////////////////
        // DO NOT ACTUALLY USE USERS' FIRST AND LAST NAMES AS 
        // IDS AND PASSWORDS IN PRODUCTION. PLEASE.
        m.register_user(u.first, u.second); 
        //////////////////////////////////////////////////////
        
        int id {m.get_user()};
        e.set_id(id);
        e.swap_arg(&arg1);
        e.write(u.first);
        e.swap_arg(&arg2);
        e.write(u.second);
    }
}

void GMTS::basic_constructors()
{
    Group_Manager g {};
    TEST_ASSERT(!g.is_valid());
    TEST_ASSERT(g.get_id()==-1);
    Group_Manager h {1};
    TEST_ASSERT(h.get_id()==1);
}

void GMTS::create_group()
{
    Group_Manager g {};
    g.create_group("Test Group", 1);
    TEST_ASSERT(g.user_is_member(1));
    TEST_ASSERT(g.user_is_admin(1));
    group_id = g.get_id();
    group_key = g.get_key();
    TEST_ASSERT(!group_key.empty());
}

void GMTS::key_resolution()
{
    Group_Manager g {group_id};
    std::string m {cmpstr(g.get_key(),group_key)};
    TEST_ASSERT_MSG(g.get_key()==group_key,m.c_str());
    Group_Manager h {group_key};
    TEST_ASSERT(g.get_id()==group_id);
}

void GMTS::add_users()
{
    Group_Manager g {group_key};
    for (int i = 1; i <= 5; ++i)
    {
        Security_Status s {g.add_user(i)};
        if (i==1)
        {
            TEST_ASSERT_MSG(s==Security_Status::err_user_exists,
                    status_strings.at(s));
        }
        else
        {
            TEST_ASSERT_MSG(s==Security_Status::ok_register, 
                    status_strings.at(s)); 
        }
        TEST_ASSERT(g.user_is_member(i));
        if (i%2)
        {
            g.promote_user(i);
            TEST_ASSERT(g.user_is_admin(i));
        }
    }
}

void GMTS::remove_users()
{
    Group_Manager g {group_id};
    for (int i = 1; i <= 5; ++i)
    {
        if (g.user_is_admin(i)) g.demote_user(i);
        g.remove_user(i);
        TEST_ASSERT(!g.user_is_admin(i));
        TEST_ASSERT(!g.user_is_member(i));
    }
}

void GMTS::user_table()
{
    Group_Manager g {group_key};
    Redis::Mogu_Query_Handler q {Prefix::user};
    int id {g.get_id()};
    for (int i = 1; i <= 5; ++i)
    {
        q.execute_query("sismember user.%d.__meta__.g %d", i, id);
        TEST_ASSERT(q.yield_response<bool>());
    }
}

int main()
{
    redisContext* r {redisConnect("localhost",6379)};
    freeReplyObject(redisCommand(r, "flushall"));
    redisFree(r);

    populate_users();
    Test::TextOutput o(Test::TextOutput::Verbose);
    GMTS test {};
    return test.run(o) ? EXIT_SUCCESS : EXIT_FAILURE;
}

