/*
 * LoginAuthenticator.cpp
 *
 *  Created on: Oct 22, 2012
 *      Author: tom
 */

#include <Security/LoginAuthenticator.h>
#include <TurnLeftLib/Utils/inlines.h>
#include <Security/Security.h>
#include <Mogu.h>
#include <Redis/RedisCore.h>

namespace Security
{

LoginAuthenticator::LoginAuthenticator
(const std::string& username, const std::string& passwd)
{
	__locks = START_STATE;
	plain_userlogin = username;
	plain_passwd = passwd;
	TurnLeft::Utils::stolower(plain_userlogin);
	enc_userlogin = Security::encrypt(plain_userlogin);

	if (user_exists()) __locks |= userid_exists;
	if (user_salted()) __locks |= userid_has_salt;
	if (authstring_exists()) __locks |= proofed_auth_string_exists;
	if (authtoken_matches()) __locks |= proofed_auth_token_matches;

}

bool LoginAuthenticator::user_exists()
{
	mApp;
	const char* cenc_login = enc_userlogin.c_str();
	app->redisExec(Mogu::Keep, "hexists %s %s"
			, __NODE_USERID_LOOKUP, cenc_login);
	if (redisReply_TRUE)
	{
		/* If the user login exists, get the internal user id */
		app->redisExec(Mogu::Keep, "hget %s %s"
				, __NODE_USERID_LOOKUP, cenc_login);
		mogu_userid = redisReply_STRING;

		/* Also, get the user's last session id */
		app->redisExec(Mogu::Keep, "hget %s %s"
				, __NODE_SESSION_LOOKUP, mogu_userid.c_str());
		last_session_id = redisReply_STRING;

		return true;
	}
	return false;
}

bool LoginAuthenticator::user_salted()
{
	mApp;
	const char* cenc_id = mogu_userid.c_str();
	app->redisCommand("hexists %s %s",
			__NODE_SALT_LOOKUP, cenc_id);
	if (redisReply_TRUE)
	{
		app->redisCommand("hget %s %s",
				__NODE_SALT_LOOKUP, cenc_id);
		salt = redisReply_STRING;
		return true;
	}
	return false;
}

bool LoginAuthenticator::authstring_exists()
{
	mApp;
	raw_auth_string = 	Security::create_raw_auth_string(
			plain_userlogin, salt, plain_passwd);
	prf_auth_string = raw_auth_string;
	app->redisExec(Mogu::Keep, "hget %s %s"
			, __NODE_COLLISION_STR_LOOKUP, mogu_userid.c_str());
	int collisions = redisReply_INT;

	for (int i = 0; i < collisions; ++i)
	{
		prf_auth_string = collision_proof(prf_auth_string);
	}
	const char* cprauth_str = prf_auth_string.c_str();
	app->redisCommand("hexists %s %s"
			, __NODE_AUTH_LOOKUP,  cprauth_str);
	if (redisReply_TRUE)
	{
		app->redisCommand("hget %s %s"
				, __NODE_AUTH_LOOKUP, cprauth_str);
		prf_auth_token = redisReply_STRING;
		return true;
	}
	return false;
}

bool LoginAuthenticator::authtoken_matches()
{
	if (last_session_id == EMPTY) return false;
	mApp;
	std::string session_node = "s."+last_session_id+"."+__META_HASH;
	const char* csession = session_node.c_str();
	app->redisCommand("hexists %s %s"
			, csession, __AUTH_HASH);
	if (!redisReply_TRUE) return false;

	app->redisCommand("hget %s %s"
			, csession, __AUTH_HASH);
	last_auth_token = redisReply_STRING;
	std::string tmp_prf_auth_token = last_auth_token;
	return last_auth_token == prf_auth_token;

}

}//namspace Security
