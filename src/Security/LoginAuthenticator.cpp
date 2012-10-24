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
	plain_userid = username;
	plain_passwd = passwd;
	TurnLeft::Utils::stolower(plain_userid);
	enc_userid = Security::encrypt(plain_userid);

	if (user_exists()) __locks |= userid_exists;
	if (user_salted()) __locks |= userid_has_salt;
	if (authstring_exists()) __locks |= proofed_auth_string_exists;
	if (authtoken_matches()) __locks |= proofed_auth_token_matches;

}

bool LoginAuthenticator::user_exists()
{
	mApp;
	app->redisCommand("hexists", __NODE_SESSION_LOOKUP, enc_userid);
	if ( (bool) Redis::getInt(app->reply()))
	{
		app->redisCommand("hget", __NODE_SESSION_LOOKUP, enc_userid);
		last_session_id = Redis::toString(app->reply());
		return true;
	}
	return false;
}

bool LoginAuthenticator::user_salted()
{
	mApp;
	app->redisCommand("hexists", __NODE_SALT_LOOKUP, enc_userid);
	if ( (bool) Redis::getInt(app->reply()))
	{
		app->redisCommand("hget", __NODE_SALT_LOOKUP, enc_userid);
		salt = Redis::toString(app->reply());
		return true;
	}
	return false;
}

bool LoginAuthenticator::authstring_exists()
{
	mApp;
	raw_auth_string = 	Security::create_raw_auth_string(
			plain_userid, salt, plain_passwd);
	prf_auth_string = raw_auth_string;
	proof_token(prf_auth_string, __NODE_COLLISION_STR_LOOKUP, enc_userid);
	app->redisCommand("hexists", __NODE_AUTH_LOOKUP,  prf_auth_string);
	if ( (bool) Redis::getInt(app->reply()))
	{
		app->redisCommand("hget", __NODE_AUTH_LOOKUP, prf_auth_string);
		prf_auth_token = Redis::toString(app->reply());
		return true;
	}
	return false;
}

bool LoginAuthenticator::authtoken_matches()
{
	mApp;
	std::string session_node = "s."+last_session_id+"."+__META_HASH;
	app->redisCommand("hexists", session_node, __AUTH_HASH);
	if (! (bool) Redis::getInt(app->reply())) return false;

	app->redisCommand("hget", session_node, __AUTH_HASH);
	last_auth_token = Redis::toString(app->reply());
	std::string tmp_prf_auth_token = last_auth_token;
	proof_token(tmp_prf_auth_token, __NODE_COLLISION_TOK_LOOKUP, enc_userid);
	return tmp_prf_auth_token == prf_auth_token;

}


void LoginAuthenticator::proof_token(
		std::string& token, std::string proof_table, const std::string& key)
{
	mApp;
	app->redisCommand("hexists", proof_table, key);
	if ((bool) Redis::getInt(app->reply()))
	{
		app->redisCommand("hget", proof_table, key);
		int proof_cycles = stoi(Redis::toString(app->reply()));
		while (proof_cycles > 0)
		{
			token = Security::collision_proof(token);
			--proof_cycles;
		}
	}
}


}//namspace Security
