#ifndef GROUPMANAGER_H_
#define GROUPMANAGER_H_

#include <declarations.h>

/* 
 * The GroupManager class determines a user's access level within a group,
 * and provides methods of determining read/write access for fields within
 * the group. The GroupManager may also promote/demote group members provided
 * the caller is a group moderator/administrator.
 */
class GroupManager {

public:
    const static size_t NO_ACCESS = 0;
    const static size_t USER_ACCESS = 1;
    const static size_t ADMIN_ACCESS = 2;
    const static std::string USER_STR = std::to_string(
            (int) MoguSyntax::user);
    const static std::string GROUP_STR = std::to_string(
            (int) MoguSyntax::group);
    const static std::string ADMIN_STR = std::to_string(
            (int) MoguSytnax::moderator);
    
    GroupManager();
    GroupManger(const std::string& group_id);
    void promote (const std::string& user_id);
    void demote (const std::string& user_id);
    bool hasReadAccess (const std::string& field_name);
    bool hasWriteAccess (const std::string& field_name);
    inline const size_t& accessLevel() const {
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
    std::string     group_id;       // The database location of the group
    size_t          access_level;   // 0 = no access, 1 = user, 2 = admin
    Redis::ContextQuery grpdb;      // Context for Group database
    Redis::ContextQuery usrdb;      // Context for user database
    Redis::ContextQuery plcdb;      // Context for policy database
};

#endif
