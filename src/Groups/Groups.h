/*
 * Groups.h
 *
 *  Created on: Nov 2, 2012
 *      Author: tom
 */

#ifndef GROUPS_H_
#define GROUPS_H_

/* This file outlines user groups for Mogu users.
 *
 * Within the redis instance, groups will be stored in keyspace "g.*".
 * At the user level, the user will have at their root node a hash
 * called 'groups', in which each key will be a groupid, and each value will
 * be their access level for that group.
 *
 * The keyspace will be constructed lke:
 * 	g.[groupid].[userlevel ...]
 *
 * Users can belong to multiple groups. Each user level included within the
 * group will have a set of userids (the hashed, encrypted strings used
 * in the session_node lookup).
 *
 * Within a group, users with a higher user level will have access to sub-users'
 * nodes that specify said permissions.
 *
 * For example, if the system has two user levels: administrators and genusers,
 * with administrators being of a higher level, any dynamic node whose policy
 * states "read : administrators" will be able to be read by admins. Likewise,
 * if a policy states "write : administrators", the same would apply.
 *
 * When writing a node, the data will be written to the user's most recent
 * session. Bearing this in mind, it is possible, if the user is provided
 * write-access to the node through the web interface, for admin set values
 * to be overwritten. If you do not wish a user to be able to modify a node,
 * simply do not give them a way to load the field that modifies its value.
 *
 * Dynamic policies support "read" and "write" permissions. However, if you
 * wish to toggle certain widgets to be loaded only be users of a certain level,
 * you must do that with whatever accessor widgets use the data. This is done
 * by adding the 'level' key to widget definitions.
 *
 * For instance, if you have the following widget;
 *
 *     widgets["foo"] = {
 *         type : container,
 *         level: "administrator"
 *     }
 *
 * The widget will only be loaded if the user is the administrator of at least
 * one of their groups. If the widget provides access to information about
 * users within that group, it will provide access to all users across all
 * groups in which that user is an administrator.
 *
 * Groups may be named upon creating by setting the "name" value of
 * the g.[groupid] hash. (hset g.[groupid] &name& "This Group's Name")
 *
 * Notable namespace info:
 *
 * g.[groupid]
 * 	hash of meta information
 * 		name : "Plaintext Group Name"
 *
 * g.[groupid].&levels&
 * 	set of levels currently contained within the group
 *
 * g.[groupid].[grouplevel]
 * 	A set of all group members who are in that level
 *
 * In order to create a group, you can use the "create_group" action, which
 * works as follows:
 *
 * {
 * 		action : "create_group"
 * 		message : "Group Name"
 * 		# if the group name is left blank, it will only be accessible via id
 * 		# the id will be held in the active user's GROUPID slot and can be
 * 		# used for some other purpose
 * }
 *
 * To add users to a group, do this:
 *
 * {
 * 		action : "slot",
 * 		listener : "USERID" #this is the slotted constant, not the user's id
 * 		message : "the user's plaintext id" # this will be encrypted
 * }
 *
 * {
 * 		action : "add_membership"
 * 		listener : "Group Name"
 * 			-OR-
 * 		listener : "groupid"
 * 			(Mogu will first check to see if the groupid exists; if not, it
 * 			will check each group's name to see if the name exists)
 * 		message : "userlevel"
 * }
 *
 * To add user levels to your Mogu application, simply add the following to
 * your mogu.conf file in the 'meta' section:
 *
 * userlevels = administrator, editor, general_user, newbie
 *
 * The levels will be ranked from first to last in descending order, meaning
 * the first item will be the most permissive, and the last will be the least.
 * By default, all users who sign up for the site will be given the least
 * permissive value. You must set up an add_membership event in order to
 * grant the user extra priveleges.
 *
 * mogu.conf does allow one default user login to be granted administrator
 * priveleges across ALL groups. Whenever a new group is created, this user
 * will be added as an administrator of that group. This will be reasserted
 * whenever your mogu server starts, and will be removed whenever your application
 * shuts down, meaning this can be changed at will, but will require a server
 * restart.
 *
 * defaultadmin = plaintext_userid (ie sysadmin@mycompany.com)
 *
 *
 */


#endif /* GROUPS_H_ */
