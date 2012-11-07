/*
 * UserManager.cpp
 *
 *  Created on: Oct 22, 2012
 *      Author: tom
 */


#include <Security/UserManager.h>
#include <Mogu.h>
#include <TurnLeftLib/Utils/inlines.h>
#include <Security/LoginAuthenticator.h>
#include <Security/Security.h>
#include <Redis/RedisUtils.h>

namespace Security{

bool UserManager::createUser(
		const std::string& plain_id, const std::string& plain_passwd)
{
	__plain_userid = plain_id;
	TurnLeft::Utils::stolower(__plain_userid);
	__enc_userid = Security::encrypt(__plain_userid);
	const char* cenc_id = __enc_userid.c_str();

	// Ensure that the user does not already exist
	if (__user_exists()) return false;

	// Generate salt and add it to the database
	__salt = generate_salt();
	const char* csalt = __salt.c_str();
	application.redisCommand("hset %s %s %s"
			, __NODE_SALT_LOOKUP, cenc_id, csalt);

	// Create an auth string for the user
	UniqueHashPackage hashPkg_auth(
			create_raw_auth_string(
					__plain_userid, __salt, plain_passwd),
					__NODE_AUTH_STR_SET);
	Redis::makeUniqueHash(hashPkg_auth);

	// Let Mogu know about any collisions that took place so they can be replicated
	if (hashPkg_auth.num_cycles > 0)
	{
		application.redisCommand("hset %s %s %d"
				, __NODE_COLLISION_STR_LOOKUP
				, cenc_id
				, hashPkg_auth.num_cycles);
	}
	const char* cprhash_auth = hashPkg_auth.proofed_hash.c_str();

	// Create an auth token for the session
	UniqueHashPackage hashPkg_token(
			Hash::toHash(plain_id + __salt + "first_session"), __NODE_AUTH_TOK_SET);
	Redis::makeUniqueHash(hashPkg_token);
	const char* cprhash_tok = hashPkg_token.proofed_hash.c_str();
	application.redisCommand("sadd %s %s",
			__NODE_AUTH_TOK_SET, cprhash_tok);
	application.freeReply();

	// Associate the auth string and the auth token:
	application.redisCommand("hset %s %s %s"
			, __NODE_AUTH_LOOKUP
			, cprhash_auth
			, cprhash_tok);
	application.freeReply();

	Security::AuthPackage authPkg;
	authPkg.encrypted_id = __enc_userid;
	authPkg.last_session = "global";
	authPkg.proofed_auth_string = hashPkg_auth.proofed_hash;
	authPkg.salt = __salt;
	application.getManager().createNewSession(authPkg);

	return true;
}

void UserManager::__get_recent_session__()
{
	__encid_nempty();
	mApp;
	app->redisCommand("hget %s %s",
			__NODE_SESSION_LOOKUP, __enc_userid.c_str());
	__recent_session = Redis::toString(app->reply());
}

bool UserManager::__user_exists()
{
	__encid_nempty();
	mApp;
	app->redisCommand("hexists %s %s",
			__NODE_SESSION_LOOKUP, __enc_userid.c_str());
	return (bool) Redis::getInt(app->reply());
}

void UserManager::__get_salt()
{
	mApp;
	app->redisCommand("hget %s %s",
			__NODE_SALT_LOOKUP, __enc_userid.c_str());
	__salt = Redis::toString(app->reply());
	if (__salt == EMPTY) return; //TODO THROW ERROR
}

bool UserManager::resetPassword(const std::string& plain_userid)
{
	TurnLeft::Utils::RandomCharSet r;
	std::string plain_newpass = r.generate(4);
	bool success = changePassword(plain_userid, plain_newpass);
}

bool UserManager::changePassword(
		const std::string& plain_userid, const std::string& plain_newpass)
{
	__enc_userid = Security::encrypt(plain_userid);
	if (!__user_exists()) return false;
	__get_salt();
	__get_recent_session__();

	return false;
}

}//namespace Security


