/* GroupManager.h
 *
 * Provides functions for managing group membership, creating, and 
 * destroying groups.
 *
 * Source: GroupManager.cpp
 *
 * Author: Tom A. Thorogood | August 2013
 */


#ifndef GROUP_MANAGER_H_
#define GROUP_MANAGER_H_

#include "../Types/SecurityStatus.h"
#include "../Redis/MoguQueryHandler.h"
#include <string>
#include <vector>

class Group_Manager
{
public:
    Group_Manager(){}
    Group_Manager(const int& group_id);
    Group_Manager(const std::string& group_key);
    Group_Manager(const std::string& group_name, const int& user_id);
    Group_Manager(const Node_Value& v);
    ~Group_Manager() { if (db) delete db; }
    
    void create_group(const std::string& group_name, const int& founder);

    bool user_is_member(const int& user_id);
    bool user_is_admin(const int& user_id);

    Security_Status add_user(const int& user_id);
    Security_Status remove_user(const int& user_id);
    Security_Status promote_user(const int& user_id);
    Security_Status demote_user(const int& user_id);

    std::string get_key();

    inline void set_id(const int& i) { id = i; }
    inline int get_id() { return id; }
    inline bool is_valid() { return id > -1; }

private:

    inline void redis_connect() 
    { if (!db)
        db = new Redis::Mogu_Query_Handler(Prefix::group); 
      else db->flush();
    }

    bool key_exists(const std::string&);
    std::vector <std::string> get_user_groups(const int& user_id);
    int get_id_from_name(const std::string& name, const int& user_id);

    int consume_id();
    std::string create_salt();
    std::string create_key(const std::string&, const std::string&);

    int id {-1};
    Redis::Mogu_Query_Handler* db {};
    std::vector <std::string> keylist {};
};

#endif
