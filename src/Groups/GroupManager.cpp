#include "GroupManager.h"
#include <Mogu.h>
GroupManager::GroupManager()
:   grpdb(Prefix::group)
    , usrdb (Prefix::user)
    , plcdb (Prefix::policies)
{}

GroupManager::GroupManager (const int& grpid)
:   group_id(grpid)
    , grpdb(Prefix::group)
    , usrdb(Prefix::user)
    , plcdb(Prefix::policies)
    , USER_STR(std::to_string((int) MoguSyntax::user))
    , GROUP_STR(std::to_string((int) MoguSyntax::group))
    , ADMIN_STR(std::to_string((int) MoguSyntax::moderator))
{
    setAccessLevel();
    grpdb.execute();
}

GroupManager::GroupManager(const std::string& group_key)
: 
    grpdb(Prefix::group)
    , usrdb(Prefix::user)
    , plcdb(Prefix::policies)
    , USER_STR(std::to_string((int) MoguSyntax::user))
    , GROUP_STR(std::to_string((int) MoguSyntax::group))
    , ADMIN_STR(std::to_string((int) MoguSyntax::moderator))
{
    /* Provided a key instead of the group id, looks up the group id
     * with this information. 
     */
    grpdb.appendQuery("hget groups.meta.keys %s", group_key.c_str());
    group_id = grpdb.yieldResponse <int>();
    if (group_id != 0) setAccessLevel();
    grpdb.execute();
}

/* This may only be executed by administrators. 
 * Promoting a user that is not already a member of the group will add this
 * member to the group. Promoting a user that is already a member will 
 * promote that member to administrator status.
 */
void GroupManager::promote (const std::string& userid)
{
    if (access_level != AccessLevel::ADMIN_ACCESS) return;
    grpdb.appendQuery("sismember groups.%d.members %s",
            group_id, userid.c_str());
    if (grpdb.yieldResponse<bool>())
        grpdb.appendQuery( "sadd groups.%d.admins %s",
                group_id, userid.c_str());
    else
        grpdb.appendQuery( "sadd groups.%d.members %s",
                group_id, userid.c_str());
    grpdb.execute();
}

/* This may only be executed by administrators.
 * Demoting a user that is an administrator will grant them member access;
 * demoting auser that is not an administrator will remove them from the group.
 */
void GroupManager::demote (const std::string& userid)
{
    if (access_level != AccessLevel::ADMIN_ACCESS) return;
    grpdb.appendQuery( "sismember groups.%d.admins %s",
            group_id, userid.c_str());
    grpdb.appendQuery( "sismember groups.%d.members %s",
            group_id, userid.c_str());
    if (grpdb.yieldResponse <bool>()) 
        grpdb.appendQuery( "srem groups.%d.admins %s", 
                group_id, userid.c_str());
    else if (grpdb.yieldResponse <bool>())
        grpdb.appendQuery( "srem groups.%d.members %s",
                group_id, userid.c_str());
    grpdb.execute();
}

bool GroupManager::hasReadAccess (const std::string& field)
{
    grpdb.appendQuery("exists groups.%d", group_id);
    
    // Nobody has read access to something that doesn't exist
    if (! grpdb.yieldResponse <bool>()) 
        return false;   
    
    plcdb.appendQuery(
            "hget policies.%s %d", field.c_str(), MoguSyntax::read);
    auto access = plcdb.yieldResponse <std::string>();
    std::string test_str = 
        // An administrator will have read access if the 
        // policy is tagged with 'moderator'
        access_level == AccessLevel::ADMIN_ACCESS ? ADMIN_STR :
        // A non-administrator will have read access if the
        // policy is tagged with 'group' 
            access_level == AccessLevel::USER_ACCESS ? GROUP_STR : USER_STR;
    // If the test string is in the access tags, read access is granted
    return access.find(test_str) != std::string::npos;
}

bool GroupManager::hasWriteAccess(const std::string& field)
{
    plcdb.appendQuery(
        "hget policies.%s %d", field.c_str(), MoguSyntax::write);

    // If the user is not a member of the group, they do not have write access
    // no matter what
    std::string access = plcdb.yieldResponse <std::string>();
    std::string test_str = 
        access_level == AccessLevel::ADMIN_ACCESS 
        ? ADMIN_STR 
        : access_level == AccessLevel::USER_ACCESS
            ? GROUP_STR
            : USER_STR;
    return (test_str != USER_STR)
        && (access.find(test_str) != std::string::npos);
}

