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

	std::string new_session_id = newSessionID();
	const char* cnewsession = new_session_id.c_str();
	std::string node = "s."+new_session_id+"."+__META_HASH;
	const char* cnode = node.c_str();
	const char* clast_session = pkg.last_session.c_str();
	const char* cenc_id = pkg.encrypted_id.c_str();

	/* Link the new session to the previous session */
	app->redisCommand("hset %s %s %s"
			, cnode, __PREV_HASH, clast_session);
	app->freeReply();

	/* Link the user id with the new session */
	app->redisCommand("hset %s %s %s",
			__NODE_SESSION_LOOKUP, cenc_id, cnewsession);
	app->freeReply();

	/* Add the new session to the session set for easy access */
	app->redisCommand("sadd %s %s"
			, __NODE_SESSION_SET, cnewsession);
	app->freeReply();

	std::string proto_auth_token = Hash::toHash(Security::encrypt(
			pkg.encrypted_id
			+ new_session_id
			+ pkg.proofed_auth_string));


	UniqueHashPackage new_auth_token(proto_auth_token, __NODE_AUTH_TOK_SET);
	Redis::makeUniqueHash(new_auth_token);
	const char* cnew_auth_tok = new_auth_token.proofed_hash.c_str();
	const char* cauth_str = pkg.proofed_auth_string.c_str();
	//Since all auth tokens will be unique, we do not need to store the collision info.
	app->redisCommand("hset %s %s %s"
			, cnode, __AUTH_HASH, cnew_auth_tok);
	app->freeReply();

	//Add the new auth token to the auth token set, and remove the previous one. */
	app->redisCommand("sadd %s %s"
			, __NODE_AUTH_TOK_SET, cnew_auth_tok);
	app->freeReply();

	// Do not attempt to remove the previous auth token if the previous session was "global"
	if (pkg.last_session != "global")
	{
		std::string query = "s."+pkg.last_session+"."+__META_HASH;
		const char* cquery = query.c_str();
		app->redisCommand("hget %s %s", cquery, __AUTH_HASH);
		std::string prev_auth = Redis::toString(app->reply());
		const char* cauth = prev_auth.c_str();
		app->redisCommand("srem %s %s"
				, __NODE_AUTH_TOK_SET, cauth);
		app->freeReply();
	}

	//Link the new auth token to the user's auth string
	app->redisCommand("hset %s %s %s",
			__NODE_AUTH_LOOKUP
			, cauth_str
			, cnew_auth_tok);
	app->freeReply();
	app->setSessionID(new_session_id);
}
