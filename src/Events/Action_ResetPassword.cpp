/*
 * Action_ResetPassword.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: tom
 */

#include <declarations.h>
#include <Security/Security.h>
#include <Sessions/Submission.h>
#include <Redis/RedisCore.h>
#include <algorithm>

namespace Events{
namespace ActionCenter{
namespace Actions{

bool change_password(std::string username, std::string new_password)
{
	using namespace Sessions::SubmissionHandler;
	using std::find;

	//First, encrypt this username so we can start the lookups.
	std::string e_username = Security::encrypt(username);

	//Make sure the user exists:
	Redis::command("hexists", __NODE_SESSION_LOOKUP, e_username);
	if (! (bool) Redis::getInt() ) return false;

	//Get the user's salt
	Redis::command("hexists", __NODE_SALT_LOOKUP, e_username);
	if (! (bool) Redis::getInt()) return false;
	Redis::command("hget", __NODE_SALT_LOOKUP, e_username);
	std::string user_salt = Redis::toString();

	//Then, get the last known session for this user:
	Redis::command("hget", __NODE_SESSION_LOOKUP, e_username);
	std::string n_last_session = Redis::toString();

	//Get the user's last auth token from their session.
	std::string meta_node = session_widget(n_last_session, __META_HASH);
	Redis::command(n_last_session, __AUTH_HASH);
	std::string auth_token = Redis::toString();

	//Get all the values from the auth node and put them into a vector
	Redis::strvector auth_vec;
	Redis::command("hgetall", __NODE_AUTH_LOOKUP);
	Redis::toVector(auth_vec);

	//Find the auth token within the vector.
	Redis::strvector::iterator iter = find(
			auth_vec.begin(), auth_vec.end(), auth_token);

	//The previous entry will be the user authstring
	std::string user_auth = *--iter;

	std::string new_auth_string = Security::create_auth_string(
			username,
			user_salt,
			new_password);

	//Set the new auth string to the old auth token:
	Redis::command("hset", __NODE_AUTH_LOOKUP, new_auth_string, auth_token);
	Redis::clear();
	//Remove the old auth string
	Redis::command("hdel", __NODE_AUTH_LOOKUP, user_auth);

}


}//namespace Actions
}//namespace ActionCenter
}//namespace Events
