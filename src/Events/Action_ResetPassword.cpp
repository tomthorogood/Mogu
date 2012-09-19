/*
 * Action_ResetPassword.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: tom
 */

#include <declarations.h>
#include <Security/Security.h>
#include <Sessions/Submission.h>
#include <TurnLeftLib/Utils/randomcharset.h>
#include <Sessions/Lookups.h>
#include <Redis/RedisCore.h>
#include <algorithm>

namespace Events{
namespace ActionCenter{
namespace Actions{

bool change_password(std::string username, std::string new_password)
{
	using namespace Sessions::SubmissionHandler;
	using namespace Sessions::Lookups;
	using std::find;

	//Make sure user exists.
	std::string e_userid = Security::encrypt(username);
	if (!hashkey_exists(__NODE_SESSION_LOOKUP,e_userid)) return false;

	std::string salt = user_salt(e_userid);

	//Get the user's auth token
	std::string usr_last_session = last_session(username);
	std::string raw_auth_tok = raw_last_authtoken(usr_last_session);
	std::string num_auth_tok_cycles = proofed_last_authtoken(e_userid);
	int i_tok_cycles = atoi(num_auth_tok_cycles.c_str());
	std::string auth_token = raw_auth_tok;
	for (int i = 0; i < i_tok_cycles; i++)
	{
		auth_token = Security::collision_proof(raw_auth_tok);
	}

	//Create the new auth string
	TokenCycles new_auth_str;
	new_auth_str.first =
			Security::create_raw_auth_string(username, salt, new_password);
	Security::proof_auth_string(&new_auth_str);

	//Set the new auth string to the current auth token
	Redis::command("hset", __NODE_AUTH_LOOKUP, new_auth_str.first, auth_token);
	Redis::clear();
	if (new_auth_str.second > 0)
	{//set the colission table if necessary
		std::string s = itoa(new_auth_str.second);
		Redis::command(
				"hset", __NODE_COLLISION_STR_LOOKUP,e_userid,s);
		Redis::clear();
	}
	else if (hashkey_exists(__NODE_COLLISION_STR_LOOKUP, e_userid))
	{//if the new auth string had no collisions, delete the entry from
		//the database node
		Redis::command("hdel", __NODE_COLLISION_STR_LOOKUP, e_userid);
		Redis::clear();
	}
	return true;
}

bool reset_password(std::string username)
{
	using TurnLeft::Utils::RandomCharSet;
	std::string e_userid = Security::encrypt(username);
	RandomCharSet rchar;
	std::string new_password = rchar.generate(8);
	if (!change_password(username,new_password)) return false;
	std::string URL = Application::mogu()->bookmarkUrl();
	EmailPacket pkt;
	pkt.to_address = get_user_email(username);
	pkt.subject = "Password Reset Request from "+URL;
	pkt.message =
			"You recently requested that your password for"
			+ URL + " be reset. Your new password is " + new_password
			+ ". You are encouraged to change this password next time you "
			+ "log in. If you did not request your password be changed, you "
			+ "should log in and change it immediately and ensure that your "
			+ "email has not been compromised. Thank you.";
	send_system_email(&pkt);
	return true;
}


}//namespace Actions
}//namespace ActionCenter
}//namespace Events
