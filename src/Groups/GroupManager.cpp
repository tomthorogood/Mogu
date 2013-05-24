#include "GroupManager.h"

GroupManager::GroupManager()
:   grpdb(Prefix::group)
    , usrdb (Prefix::user)
    , plcdb (Prefix::policies)
    , group_id(EMPTY)
{}

GroupManager::GroupManger (const std::string& group_id)
:   grpdb(Prefix::group)
    , usrdb(Prefix::user)
    , plcdb(Prefix::policies)
    , group_id(grpid)
{
    mApp;
    std::string user = app->getUser();
    CreateQuery(grpdb, "sismember groups.%s.admins %s",
            group_id.c_str(), user.c_str());
    CreateQuery(grpdb, "sismember groups.%s.members %s", 
            group_id.c_str(), user.c_str());

    access_level = 
        grpdb.yieldResponse <bool>() ? ADMIN_ACCESS : 
            grpdb.yieldResponse <bool>() ? USER_ACCESS : NO_ACCESS;
    grpdb.execute();
}

/* This may only be executed by administrators. 
 * Promoting a user that is not already a member of the group will add this
 * member to the group. Promoting a user that is already a member will 
 * promote that member to administrator status.
 */
void GroupManager::promote (const std::string& userid)
{
    if (access_level != ADMIN_ACCESS) return;
    CreateQuery(grpdb, "sismember groups.%s.members %s",
            group_id.c_str(), userid.c_str());
    if (grpdb.yieldResponse<bool>())
        CreateQuery(grpdb, "sadd groups.%s.admins %s",
                group_id.c_str(), userid.c_str());
    else
        CreateQuery(grpdb, "sadd grups.%s.members %s",
                group_id.c_str(), userid.c_str());
    grpdb.execute();
}

/* This may only be executed by administrators.
 * Demoting a user that is an administrator will grant them member access;
 * demoting auser that is not an administrator will remove them from the group.
 */
void GroupManager::demote (const std::string& userid)
{
    if (access_level != ADMIN_ACCESS) return;
    CreateQuery(grpdb, "sismember groups.%s.admins %s",
            group_id.c_str(), userid.c_str());
    CreateQuery(grpdb, "sismember groups.%s.members %s",
            group_id.c_str(), userid.c_str());
    if (grpdb.yieldResponse <bool>()) 
        CreateQuery(grpdb, "srem groups.%s.admins %s", 
                group_id.c_str(), userid.c_str());
    else if (grpdb.yieldResponse <bool>())
        CreateQuery(grpdb, "srem groups.%s.members %s",
                group_id.c_str(), userid.c_str());
    grpdb.execute();
}

bool GroupManager::hasReadAccess (const std::string& field)
{
    CreateQuery(grpdb,
            "exists groups.%s", group_id.c_str());
    
    // Nobody has read access to something that doesn't exist
    if (! grpdb.yieldResponse <bool>()) 
        return false;   
    
    CreateQuery(plcdb,
            "hget policies.%s %d", field.c_str(), MoguSyntax::read);
    auto access = plcdb.yieldResponse <std::string>();
    std::string test_str = 
        // An administrator will have read access if the 
        // policy is tagged with 'moderator'
        access_level == ADMIN_ACCESS ? ADMIN_STR :
        // A non-administrator will have read access if the
        // policy is tagged with 'group' 
            access_level == USER_ACCESS ? GROUP_STR : USER_STR;
    // If the test string is in the access tags, read access is granted
    return access.find(test_str) != std::string::npos;
}

bool GroupManager::hasWriteAccess(const std::string& field)
{
    CreateQuery(plcdb,
            "hget policies.%s %d", field.c_str(), MoguSyntax::write);

    // If the user is not a member of the group, they do not have write access
    // no matter what
    auto access = plcdb.yieldResponse <std::string>();
    std::string test_str = 
        access_level == ADMIN_ACCESS ? ADMIN_STR :
            access_level == USER_ACCESS ? GROUP_STR : USER_STR;
    return (test_str != USER_STR) && (access.find(test_str) != std::string::npos);
}

#endif // GROUPMANGER_H_
