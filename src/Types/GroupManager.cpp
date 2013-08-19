#include "GroupManager.h"
#include "../Types/Prefixes.h"
#include "../Redis/QueryHandler/QueryHandler.h"
#include <vector>
#include <algorithm>

Group_Manager::Group_Manager(const int& id_)
    : id(id_)
{}

Group_Manager::Group_Manager(const std::string& key)
{
    redis_connect();
    db->.append_query("hexists group.__meta__.keys %s", key.c_str());
    if (db->yield_response<bool>())
    {
        db->append_query("hget group.__meta__.keys %s", key.c_str());
        id = db->yield_response<int>();
    }
    else db->flush();
}

bool Group_Manager::user_is_member(const int& u)
{
    redis_connect();
    if (!is_valid()) return false;
    db->append_query("sismember group.%d.__meta__.m %s", id, u);
    return db->yield_response<bool>();
}

bool Group_Manager::user_is_admin(const int& u)
{
    redis_connect();
    if (!is_valid()) return false;
    db->append_query("sismember group.%d.__meta__.a %s", id, u);
    return db->yield_response<bool>();
}

std::string Group_Manager::get_key()
{
    redis_connect();
    if (!is_valid) return "";

    db->append_query("hget group.%d.__meta__ k", id);
    return db->yield_response<std::string>();
}

Security_Status Group_Manager::add_user(const int& u)
{
    redis_connect();
    if (!is_valid()) return Security_Status::err_no_group_selected;
    if (user_exists(u)) return Security_Status::err_user_exists;
    db->execute_query("sadd group.%d.__meta__.m %d", id, u);


    // Also add the group to the user's grup list.
    Redis::Query_Handler q {Prefix::user};
    q.execute_query("sadd user.%d.__meta__.g %d", u, id);
    return Security_Status::ok_register;
}

Security_Status Group_Manager::promote_user(const int& u)
{
    redis_connect();
    if (!is_valid()) return Security_Status::err_no_group_selected;
    if (!user_exists(u)) return Security_Status::err_user_not_found;
    db->execute_query("sadd group.%d.__meta__.a %d", id, u);
    return Security_Status::ok_register;
}

Security_Status Group_Manager::remove_user(const int& u)
{
    redis_connect();
    if (!is_valid()) return Security_Status::err_no_group_selected;
    if (!user_exists(u)) return Security_Status::err_user_not_found;
    if (user_is_admin(u)) demote_user(u);
    db->execute_query("srem group.%d.__meta__.m %d", id, u);

    // Also remove the group from the user's group list
    Redis::Query_Handler q {Prefix::user};
    q.execute_query("srem user.%d.__meta__.g %d", u, id);

    return Security_Status::ok_remove;
}

Security_Status Group_Manager::demote_user(const int& u)
{
    redis_connect();
    if (!is_valid()) return Security_Status::err_no_group_selected;
    if (!user_is_admin(u) || !user_exists(u)) return Security_Status::err_user_not_found;
    db->execute_query("srem group.%d.__meta__.a %d", id, u);
    return Security_Status::ok_remove;
}
