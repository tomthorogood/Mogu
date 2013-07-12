#ifndef GROUPCONTROLLER_H_
#define GROUPCONTROLLER_H_

#include <declarations.h>
#include <Groups/GroupManager.h>

/* The GroupController is how the application itself interacts
 * with groups, such as creating, destroying them, adding members
 * accessing member fields, and so forth.
 */

class GroupController {
private:
    int group_id;
    std::vector <std::string> members_v;
    std::vector <std::string> admins_v;

    // Retrieve a list fo all members of a set.
    void getSet (const char*, std::vector <std::string>&);

    // Consume a group id from either the current group count or
    // from the list of abandoned ids.
    int consumeGroupId(Redis::ContextQuery&);
    GroupManager manager;

public:
    GroupController(); // Constructor for creating groups
    GroupController(const int& group_id); // Constructor for interaction
    const std::vector <std::string>& members();
    const std::vector <std::string>& administrators();
    void createGroup (const std::string& group_name, bool userIsAdmin);
    inline int getGroupId(const std::string& group_name)
    {
        const char* c_name = group_name.c_str();
        Redis::ContextQuery db(Prefix::meta);
        db.appendQuery("hget meta.groups %s", c_name);
        std::string g_id = db.yieldResponse <std::string>();
        return std::atoi(g_id.c_str());
    }
    void destroyGroup ();

    void joinGroup ();

    // Purge the member list and requery in order to get a more updated
    // list
    inline const std::vector <std::string>& updateMembers() {
        members_v.clear();
        return members();
    }

    inline const std::vector <std::string>& updateAdministrators() {
        admins_v.clear();
        return administrators();
    }

};

#endif
