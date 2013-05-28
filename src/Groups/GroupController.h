#ifndef GROUPCONTROLLER_H_
#define GROUPCONTROLLER_H_

#include <delcarations.h>

/* The GroupController is how the application itself interacts
 * with groups, such as creating, destroying them, adding members
 * accessing member fields, and so forth.
 */

class GroupController {
public:
    GroupController(); // Constructor for creating groups
    GroupController(const std::string& group_id); // Constructor for interaction
    const std::vector <std::string>& members();
    const std::vector <std::string>& administrators();
    void createGroup (const std::string& group_name, bool userIsAdmin);
    void destroyGroup ();

    // Allows a member to join the group without being manually added
    // by an administrator, provided they have the authorization code
    // specified at groups.[group_id].auth
    void joinGroup (const std::string& authorization);

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

private:
    std::string group_id;
    std::vector <std::string> members_v;
    std::vector <std::string> admins_v;

    // Retrieve a list fo all members of a set.
    void getSet (const char*);
    GroupManager manager;
};

#endif
