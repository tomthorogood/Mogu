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
#include <TurnLeftLib/Utils/inlines.h>
#include <Sessions/Lookups.h>
#include <Redis/RedisCore.h>
#include <algorithm>
#include <Static.h>
#include <Mogu.h>
#include <Events/MoldableActionCenter.h>

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
	std::string auth_token = raw_auth_tok;
	if (num_auth_tok_cycles != EMPTY)
	{
		int i_tok_cycles = atoi(num_auth_tok_cycles.c_str());

		for (int i = 0; i < i_tok_cycles; i++)
		{
			auth_token = Security::collision_proof(raw_auth_tok);
		}
	}

	//Create the new auth string
	TokenCycles new_auth_str;
	new_auth_str.first =
			Security::create_raw_auth_string(username, salt, new_password);
	Security::proof_auth_string(&new_auth_str);

	mApp;

	//Set the new auth string to the current auth token
	app->redisCommand("hset", __NODE_AUTH_LOOKUP, new_auth_str.first, auth_token);
	if (new_auth_str.second > 0)
	{//set the colission table if necessary
		std::string s = itoa(new_auth_str.second);
		app->redisCommand(
				"hset", __NODE_COLLISION_STR_LOOKUP,e_userid,s);
	}
	else if (hashkey_exists(__NODE_COLLISION_STR_LOOKUP, e_userid))
	{//if the new auth string had no collisions, delete the entry from
		//the database node
		app->redisCommand("hdel", __NODE_COLLISION_STR_LOOKUP, e_userid);
	}
	return true;
}

bool reset_password(std::string username)
{
	using TurnLeft::Utils::RandomCharSet;
	TurnLeft::Utils::stolower(username);
	std::string e_userid = Security::encrypt(username);
	RandomCharSet rchar;
	std::string new_password = rchar.generate(8);
#ifdef DEBUG
	std::cout << "NEW PASSWORD: " << new_password << std::endl;
#endif
	if (!change_password(username,new_password)) return false;
	mApp;
	std::string URL = app->bookmarkUrl();
	EmailPacket pkt;
#ifdef DEBUG
	std::cout << get_user_email(username) << std::endl;
#endif
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
