/*
 * ApplicationManager.cpp
 *
 *  Created on: Oct 21, 2012
 *      Author: tom
 */

#include <Types/ApplicationManager.h>
#include <TurnLeftLib/Utils/randomcharset.h>
#include <Security/LoginAuthenticator.h>
#include <Security/Security.h>
#include <Redis/MoguQuery.h>
#include <Redis/RedisUtils.h>
#include <Mogu.h>
#include <hash.h>

using namespace TurnLeft::Utils;

static const std::string INIT_SESSION = "global";

ApplicationManager::ApplicationManager(Mogu& app) : application(app)
{
	application.setSessionID(INIT_SESSION);
}

std::string ApplicationManager::newSessionID()
{
	RandomCharSet r;
	std::string baseID = r.generate(8);
	std::string hashedID = Hash::toHash(baseID);
	UniqueHashPackage p(hashedID, __NODE_SESSION_SET);
	Redis::makeUniqueHash(p);
	return p.proofed_hash;
}

bool ApplicationManager::userLogin(
		const std::string& plain_userID, const std::string& userauth)
{
	Security::LoginAuthenticator auth(plain_userID, userauth);
	if (!auth.authenticated()) return false;
	Security::AuthPackage authPkg;
	//Avoids having to re-retrieve data that we'll need again.
	auth.fillAuthPackage(authPkg);
	createNewSession(authPkg);
	return true;
}

void ApplicationManager::createNewSession(Security::AuthPackage& pkg)
{
	mApp;
	TokenCycles authtoken_info;
	Redis::MoguQuery q(app->DBConnection());

	std::string new_session_id = newSessionID();
	std::string node = "s."+new_session_id+"."+__META_HASH;

	/* Link the new session to the previous session */
	app->redisCommand("hset", node, __PREV_HASH, pkg.last_session);
	app->freeReply();

	/* Link the user id with the new session */
	app->redisCommand(
			"hset", __NODE_SESSION_LOOKUP, pkg.encrypted_id, new_session_id);
	app->freeReply();

	/* Add the new session to the session set for easy access */
	app->redisCommand(
			"sadd", __NODE_SESSION_SET, new_session_id);
	app->freeReply();

	std::string proto_auth_token = Hash::toHash(Security::encrypt(
			pkg.encrypted_id
			+ new_session_id
			+ pkg.proofed_auth_string));


	UniqueHashPackage new_auth_token(proto_auth_token, __NODE_AUTH_TOK_SET);
	Redis::makeUniqueHash(new_auth_token);
	//Since all auth tokens will be unique, we do not need to store the collision info.
	app->redisCommand("hset", node, __AUTH_HASH, new_auth_token.proofed_hash);
	app->freeReply();

	//Add the new auth token to the auth token set, and remove the previous one. */
	app->redisCommand("sadd", __NODE_AUTH_TOK_SET, new_auth_token.proofed_hash);
	app->freeReply();

	// Do not attempt to remove the previous auth token if the previous session was "global"
	if (pkg.last_session != "global")
	{
		std::string prev_auth = q.getSessionMetaValue(pkg.last_session, __AUTH_HASH);
		app->redisCommand("srem", __NODE_AUTH_TOK_SET, prev_auth);
		app->freeReply();
	}

	//Link the new auth token to the user's auth string
	app->redisCommand("hset",
			__NODE_AUTH_LOOKUP
			, pkg.proofed_auth_string
			, new_auth_token.proofed_hash);
	app->freeReply();
	app->setSessionID(new_session_id);
}


