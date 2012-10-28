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
	const char* cenc_id = enc_userid.c_str();
	app->redisCommand("hexists %s %s"
			, __NODE_SESSION_LOOKUP, cenc_id);
	if ( (bool) Redis::getInt(app->reply()))
	{
		app->redisCommand("hget %s %s"
				, __NODE_SESSION_LOOKUP, cenc_id);
		last_session_id = Redis::toString(app->reply());
		return true;
	}
	return false;
}

bool LoginAuthenticator::user_salted()
{
	mApp;
	const char* cenc_id = enc_userid.c_str();
	app->redisCommand("hexists %s %s",
			__NODE_SALT_LOOKUP, cenc_id);
	if ( (bool) Redis::getInt(app->reply()))
	{
		app->redisCommand("hget %s %s",
				__NODE_SALT_LOOKUP, cenc_id);
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
	const char* cprauth_str = prf_auth_string.c_str();
	proof_token(prf_auth_string, __NODE_COLLISION_STR_LOOKUP, enc_userid);
	app->redisCommand("hexists %s %s"
			, __NODE_AUTH_LOOKUP,  cprauth_str);
	if ( (bool) Redis::getInt(app->reply()))
	{
		app->redisCommand("hget %s %s"
				, __NODE_AUTH_LOOKUP, cprauth_str);
		prf_auth_token = Redis::toString(app->reply());
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
	if (! (bool) Redis::getInt(app->reply())) return false;

	app->redisCommand("hget %s %s"
			, csession, __AUTH_HASH);
	last_auth_token = Redis::toString(app->reply());
	std::string tmp_prf_auth_token = last_auth_token;
	proof_token(tmp_prf_auth_token, __NODE_COLLISION_TOK_LOOKUP, enc_userid);
	return tmp_prf_auth_token == prf_auth_token;

}


void LoginAuthenticator::proof_token(
		std::string& token, std::string proof_table, const std::string& key)
{
	mApp;
	const char* cproof = proof_table.c_str();
	const char* ckey = key.c_str();
	app->redisCommand("hexists %s %s"
			, cproof, ckey);
	if ((bool) Redis::getInt(app->reply()))
	{
		app->redisCommand("hget %s %s"
				, cproof, ckey);
		int proof_cycles = stoi(Redis::toString(app->reply()));
		while (proof_cycles > 0)
		{
			token = Security::collision_proof(token);
			--proof_cycles;
		}
	}
}


}//namspace Security
