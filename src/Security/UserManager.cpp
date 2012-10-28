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
	plain_userid = plain_id;
	TurnLeft::Utils::stolower(plain_userid);
	enc_userid = Security::encrypt(plain_userid);
	const char* cenc_id = enc_userid.c_str();

	// Ensure that the user does not already exist
	application.redisCommand("hexists %s %s"
			, __NODE_SESSION_LOOKUP, cenc_id);
	if ( (bool) Redis::getInt(application.reply())) return false;

	// Generate salt and add it to the database
	salt = generate_salt();
	const char* csalt = salt.c_str();
	application.redisCommand("hset %s %s %s"
			, __NODE_SALT_LOOKUP, cenc_id, csalt);

	// Create an auth string for the user
	UniqueHashPackage hashPkg_auth(
			create_raw_auth_string(
					plain_userid, salt, plain_passwd),
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
			Hash::toHash(plain_id + salt + "first_session"), __NODE_AUTH_TOK_SET);
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
	authPkg.encrypted_id = enc_userid;
	authPkg.last_session = "global";
	authPkg.proofed_auth_string = hashPkg_auth.proofed_hash;
	authPkg.salt = salt;
	application.getManager().createNewSession(authPkg);

	return true;
}

}//namespace Security


