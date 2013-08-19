#ifndef GROUP_MANAGER_H_
#define GROUP_MANAGER_H_

#include "../Types/SecurityStatus.h"


class Group_Manager
{
public:
    Group_Manager(){}
    Group_Manager(const int& group_id);
    Group_Manager(const std::string& group_key);
    ~GroupManager() { 
        if (db) delete db;
    }
    
    int create_group(const std::string& group_name);
    
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

    inline bool redis_connect() 
        { if (!db) db = new Redis::Query_Handler(Prefix::group); }

    bool key_exists();

    int consume_id();
    std::string create_salt();
    std::string create_key();


    int id {-1};
    Redis::Query_Handler* db {};
    std::vector <std::string> keylist {};

};

#endif
