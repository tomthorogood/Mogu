#include "GroupManager.h"
#include "../Types/Prefixes.h"
#include "../Redis/QueryHandler/QueryHandler.h"
#include "../hash.h"
#include "../Config/inline_utils.h"
#include <TurnLeftLib/Utils/randomcharset.h>
#include <vector>
#include <algorithm>

#include <cassert>

Group_Manager::Group_Manager(const int& id_)
    : id(id_)
{}

Group_Manager::Group_Manager(const std::string& key)
{
    redis_connect();
    std::string k {stripquotes(key)};
    db->append_query("hexists group.__meta__.keys %s", k.c_str());
    if (db->yield_response<bool>())
    {
        db->append_query("hget group.__meta__.keys %s", k.c_str());
        id = db->yield_response<int>();
    }
    else db->flush();
}

Group_Manager::Group_Manager(const std::string& name, const int& user)
{
    id = get_id_from_name(name,user);
}

std::vector <std::string> Group_Manager::get_user_groups(const int& user)
{
    Redis::Mogu_Query_Handler q {Prefix::user};
    q.execute_query("smembers user.%d.__meta__.g", user);
    return q.yield_response <std::vector<std::string>>();
}

Group_Manager::Group_Manager(const Node_Value& v)
{
    if (v.is_int()) id = v.get_int();
    else if (v.is_string())
    {
        std::string key {v.get_string()};
        key = stripquotes(key);
        redis_connect();
        db->append_query("hexists group.__meta__.keys %s", key.c_str());
        if (db->yield_response<bool>())
        {
            db->append_query("hget group.__meta__.keys %s", key.c_str());
            id = db->yield_response<int>();
        }
        else db->flush();
    }
}

bool Group_Manager::user_is_member(const int& u)
{
    redis_connect();
    if (!is_valid()) return false;
    db->execute_query("sismember group.%d.__meta__.m %d", id, u);
    return db->yield_response<bool>();
}

bool Group_Manager::user_is_admin(const int& u)
{
    redis_connect();
    if (!is_valid()) return false;
    db->execute_query("sismember group.%d.__meta__.a %d", id, u);
    return db->yield_response<bool>();
}

std::string Group_Manager::get_key()
{
    redis_connect();
    if (!is_valid()) return "";
    db->execute_query("hget group.%d.__meta__ k", id);
    return db->yield_response<std::string>();
}

Security_Status Group_Manager::add_user(const int& u)
{
    redis_connect();
    if (!is_valid()) return Security_Status::err_no_group_selected;
    if (user_is_member(u)) return Security_Status::err_user_exists;
    db->execute_query("sadd group.%d.__meta__.m %d", id, u);

    // Also add the group to the user's grup list.
    Redis::Mogu_Query_Handler q {Prefix::user};
    q.execute_query("sadd user.%d.__meta__.g %d", u, id);
    return Security_Status::ok_register;
}

Security_Status Group_Manager::promote_user(const int& u)
{
    redis_connect();
    if (!is_valid()) return Security_Status::err_no_group_selected;
    if (!user_is_member(u)) return Security_Status::err_user_not_found;
    db->execute_query("sadd group.%d.__meta__.a %d", id, u);
    return Security_Status::ok_register;
}

Security_Status Group_Manager::remove_user(const int& u)
{
    redis_connect();
    if (!is_valid()) return Security_Status::err_no_group_selected;
    if (!user_is_member(u)) return Security_Status::err_user_not_found;
    if (user_is_admin(u)) demote_user(u);
    db->execute_query("srem group.%d.__meta__.m %d", id, u);

    // Also remove the group from the user's group list
    Redis::Mogu_Query_Handler q {Prefix::user};
    q.execute_query("srem user.%d.__meta__.g %d", u, id);

    return Security_Status::ok_remove;
}

Security_Status Group_Manager::demote_user(const int& u)
{
    redis_connect();
    if (!is_valid()) return Security_Status::err_no_group_selected;
    if (!user_is_admin(u) || !user_is_member(u)) return Security_Status::err_user_not_found;
    db->execute_query("srem group.%d.__meta__.a %d", id, u);
    return Security_Status::ok_remove;
}

int Group_Manager::consume_id()
{
    redis_connect();
    db->append_query("scard group.__meta__.abandoned");
    if (db->yield_response<int>())
        db->append_query("smember group.__meta__.abandoned");
    else
        db->append_query("incr group.__meta__.count");
    return db->yield_response<int>();
}

std::string Group_Manager::create_salt()
{
    TurnLeft::Utils::RandomCharSet r {};
    return r.generate(1);
}

int Group_Manager::get_id_from_name(const std::string& name, const int& user)
{
    redis_connect();
    std::string n {stripquotes(name)};
    std::vector <std::string>&& v {get_user_groups(user)};
    for (std::string g : v)
    {
        int i {atoi(g.c_str())};
        db->execute_query("hget group.%d.__meta__ n", i);
        if (db->yield_response<std::string>()==name)
            return i;
    }
    return -1;
}

bool Group_Manager::key_exists(const std::string& k)
{
    redis_connect();
    if (!keylist.size())
    {
        db->execute_query("hvals group.__meta__.keys");
        keylist = db->yield_response<std::vector<std::string>>();
    }
    return std::count(keylist.begin(),keylist.end(),k) > 0;
}

/* A group key is used to allow users to join the group, and is automatically
 * generated (although it does not *HAVE* to be used.
 *
 * The seed and salt are hashed together to create a long hash string. However,
 * by default only the first 8 characters are used, since the keys should be
 * human-typable with minimal typos. More characters from the hash are added
 * if they are necessary.
 */
std::string Group_Manager::create_key(
        const std::string& seed, const std::string& salt)
{
    std::string salted {seed + salt};
    std::string h = Hash::to_hash(salted);

    for (size_t i = 8; i < h.size(); ++i)
    {
        std::string s {h.substr(0,i)};
        if (!key_exists(s))
        {
            h = s;
            break;
        }
    }
    return h;
}

void Group_Manager::create_group(
    const std::string& group_name, const int& founding_user)
{
    // A user may not create a group if it has the same name as another
    // group of which they are already a member.
    if (get_id_from_name(group_name, founding_user) != -1) return;

    std::string salt {create_salt()};
    std::string key {create_key(group_name, salt)};
    id = consume_id();
    assert(id > -1);
    db->append_query("hset group.%d.__meta__ k %s", id, key.c_str());
    db->append_query("hset group.%d.__meta__ n %s", id, group_name.c_str());
    db->append_query("hset group.%d.__meta__ s %s", id, salt.c_str());
    db->append_query("hset group.__meta__.keys %s %d", key.c_str(), id);
    db->flush();
    add_user(founding_user);
    promote_user(founding_user);
    assert(user_is_member(founding_user));
}
