#include <Mogu.h>
#include <Redis/ContextQuery.h>
#include "GroupController.h"
#include <Security/Encryption.h>

GroupController::GroupController()
: group_id(0)
{

}

GroupController::GroupController (const int& groupid)
: group_id(groupid)
  , manager(groupid)
{
    
}

void GroupController::getSet(const char* setname, 
        std::vector <std::string>& results)
{
    if (group_id != 0 && results.size() == 0) {
        Redis::ContextQuery& db = manager.getContext(Prefix::group);
        db.appendQuery( "smembers groups.%d.%s", group_id, setname);
        results = db.yieldResponse <std::vector <std::string>>();
    }
}

const std::vector <std::string>& GroupController::members()
{
    getSet("members", members_v);
    return members_v;
}

const std::vector <std::string>& GroupController::administrators()
{
    getSet("admins", admins_v);
    return admins_v;
}


void GroupController::createGroup (const std::string& group_name,
        bool userIsAdmin=false)
{
    mApp;
    int user_id = app->getUser();
    // Assign an id to the new group
    Redis::ContextQuery& db = manager.getContext(Prefix::group);
    db.appendQuery( "incr groups");
    group_id = db.yieldResponse <int>();
    db.appendQuery( "hset groups.%d.meta name %s", 
            group_id, group_name.c_str());

    if (userIsAdmin) 
        db.appendQuery( "sadd groups.%d.admins %d", group_id, user_id);
    else
        db.appendQuery( "sadd groups.%d.members %d", group_id, user_id);
    db.execute();
}

// This may only be done through the Mogu engine if the current user is listed
// as an administrator
void GroupController::destroyGroup() {
    if (manager.accessLevel() < AccessLevel::ADMIN_ACCESS) return;
    // First, we have to remove this group from the list of available groups
    // for all of its members and administrators
    Redis::ContextQuery& db = manager.getContext(Prefix::user);
    for (std::string member : members())
    {
        db.appendQuery( "srem users.%s.groups %d", member.c_str(), group_id);
    }
    for (std::string admin : administrators())
    {
        db.appendQuery( "srem users.%s.groups %d", admin.c_str(), group_id);
    }
    db.execute();
}

bool GroupController::joinGroup(const std::string& auth_code)
{
    mApp;
    int user = app->getUser();
    Redis::ContextQuery& gdb = manager.getContext(Prefix::group);
    Redis::ContextQuery& udb = manager.getContext(Prefix::user);
    gdb.appendQuery( "hget groups.%d.meta auth", group_id);
    if (auth_code != Security::decrypt(gdb.yieldResponse <std::string>()))
        return false;
    udb.appendQuery( "sadd users.%d.groups %d", user, group_id);
    gdb.appendQuery( "sadd groups.%d.members %d", group_id, user);
    udb.execute();
    gdb.execute();
    return true;
}
