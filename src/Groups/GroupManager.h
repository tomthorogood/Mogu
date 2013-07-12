#ifndef GROUPMANAGER_H_
#define GROUPMANAGER_H_


#include <declarations.h>
#include <Redis/ContextQuery.h>

/* 
 * The GroupManager class determines a user's access level within a group,
 * and provides methods of determining read/write access for fields within
 * the group. The GroupManager may also promote/demote group members provided
 * the caller is a group moderator/administrator.
 */
enum class AccessLevel {
    NO_ACCESS       =0
    , USER_ACCESS   =1
    , ADMIN_ACCESS  =2
};

class GroupManager {

public:
    GroupManager();
    GroupManager(const int& group_id);
    GroupManager(const std::string& group_key);
    void promote (const std::string& user_id);
    void demote (const std::string& user_id);
    bool hasReadAccess (const std::string& field_name);
    bool hasWriteAccess (const std::string& field_name);
    inline const AccessLevel& accessLevel() const {
        return access_level;
    }

    inline Redis::ContextQuery& getContext(Prefix prefix) {
        switch(prefix) {
            case Prefix::group: return grpdb;
            case Prefix::user: return usrdb;
            case Prefix::policies: return plcdb;
            default: return grpdb;
        };
    }
private:
    int             group_id;       // The database location of the group
    AccessLevel     access_level;   // 0 = no access, 1 = user, 2 = admin
    Redis::ContextQuery grpdb;      // Context for Group database
    Redis::ContextQuery usrdb;      // Context for user database
    Redis::ContextQuery plcdb;      // Context for policy database
    std::string USER_STR;
    std::string GROUP_STR;
    std::string ADMIN_STR;

    inline void setAccessLevel()
    {
        mApp;
        int user = app->getUser();
        grpdb.appendQuery("sismember groups.%d.__meta__.admins %d",
                group_id, user);
        grpdb.appendQuery("sismember groups.%d.__meta__.members %d", 
                group_id, user);
         
        access_level = 
            grpdb.yieldResponse <bool>() 
                ? AccessLevel::ADMIN_ACCESS 
                : grpdb.yieldResponse <bool>() 
                    ? AccessLevel::USER_ACCESS
                    : AccessLevel::NO_ACCESS;
    } 
};

#endif
