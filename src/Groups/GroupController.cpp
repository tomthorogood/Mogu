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
        db.appendQuery( "smembers groups.%d.__meta__.%s", group_id, setname);
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


int GroupController::consumeGroupId(Redis::ContextQuery& db)
{
    db.appendQuery("scard groups.meta.abandoned");
    if (db.yieldResponse <int>() > 0)
    {
        db.appendQuery("spop groups.meta.abandoned");
        return db.yieldResponse <int>();
    }
    db.appendQuery("incr groups.meta.count");
    return db.yieldResponse <int>();
}

void GroupController::createGroup (const std::string& group_name,
        bool userIsAdmin=false)
{
    mApp;
    int user_id = app->getUser();
    // Assign an id to the new group
    Redis::ContextQuery& db = manager.getContext(Prefix::group);
    
    group_id = consumeGroupId(db);
    db.appendQuery( "hset groups.%d.__meta__ name %s", 
            group_id, group_name.c_str());

    if (userIsAdmin) 
        db.appendQuery( "sadd groups.%d.__meta__.admins %d", group_id, user_id);
    else
        db.appendQuery( "sadd groups.%d.__meta__.members %d", group_id, user_id);
    Redis::ContextQuery& usr = manager.getContext(Prefix::user);
    usr.appendQuery("sadd user.%d.__meta__.groups %d", user_id, group_id);
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
        db.appendQuery(
                "srem users.%s.__meta__.groups %d", member.c_str(), group_id);
    }
    for (std::string admin : administrators())
    {
        db.appendQuery(
                "srem users.%s.__meta__.groups %d", admin.c_str(), group_id);
    }

    db.execute();
}

void GroupController::joinGroup()
{
    mApp;
    int user = app->getUser();
    Redis::ContextQuery& gdb = manager.getContext(Prefix::group);
    Redis::ContextQuery& udb = manager.getContext(Prefix::user);
    
    gdb.appendQuery("sadd groups.%d.__meta__.members %d", 
        group_id, user);
    udb.appendQuery("sadd user.%d.__meta__.groups %d", 
        user, group_id);
    gdb.execute();
    udb.execute();
}
