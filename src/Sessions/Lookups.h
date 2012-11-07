/*
 * lookups.h
 *
 *  Created on: Sep 19, 2012
 *      Author: tom
 */

#ifndef LOOKUPS_H_
#define LOOKUPS_H_

#include <declarations.h>
#include <Redis/RedisCore.h>
#include <Security/Security.h>
#include <hash.h>
#include <Wt/WApplication>
#include <Mogu.h>
namespace Sessions{
namespace Lookups{

inline std::string session_node(std::string sessionid,std::string plaintext)
{
	return "s."+sessionid+"."+Hash::toHash(plaintext);
}

inline std::string prhshd_session_node(std::string sessionid, std::string hsh)
{
	return "s."+sessionid+"."+hsh;
}

inline bool auth_string_exists(std::string str)
{
	mApp;
	const char* cstr = str.c_str();
	app->redisCommand("hexists %s %s", __NODE_AUTH_LOOKUP, cstr);
	return (bool) Redis::getInt(app->reply());
}

inline bool session_exists(std::string sessionid)
{
	std::string checknode = prhshd_session_node(sessionid, __META_HASH);
	const char* cnode = checknode.c_str();
	mApp;
	app->redisCommand("exists %s", cnode);
	return (bool) Redis::getInt(app->reply());
}

inline bool hashkey_exists(std::string node, std::string field)
{
	mApp;
	const char* cnode = node.c_str();
	const char* cfield = field.c_str();
	app->redisCommand("hexists %s %s", cnode, cfield);
	return (bool) Redis::getInt(app->reply());
}

inline std::string last_session(std::string plain_userid)
{
	std::string e_userid = Security::encrypt(plain_userid);
	const char* cenc_id = e_userid.c_str();
	if (!hashkey_exists(__NODE_SESSION_LOOKUP, e_userid)) return EMPTY;
	mApp;
	app->redisCommand("hget %s %s", __NODE_SESSION_LOOKUP, cenc_id);
	return Redis::toString(app->reply());
}

inline std::string user_salt(std::string e_userid)
{
	mApp;
	const char* cenc_id = e_userid.c_str();
	app->redisCommand("hget %s %s", __NODE_SALT_LOOKUP, cenc_id);
	return Redis::toString(app->reply());
}

inline std::string linked_session(std::string sessionid)
{
	std::string meta_node = prhshd_session_node(sessionid, __META_HASH);
	const char* cmeta = meta_node.c_str();
	if (!hashkey_exists(meta_node, __PREV_HASH)) return EMPTY;
	mApp;
	app->redisCommand("hget %s %s", cmeta, __PREV_HASH);
	return Redis::toString(app->reply());
}

inline std::string st(std::string session)
{
	std::string meta_node = session_node(session, "meta");
	const char* cmeta = meta_node.c_str();
	mApp;
	app->redisCommand("hget %s %s", cmeta, __AUTH_HASH);
	return Redis::toString(app->reply());
}

inline std::string proofed_last_authtoken(std::string proofed_authstring)
{
	if (!hashkey_exists(__NODE_COLLISION_TOK_LOOKUP,proofed_authstring))
		return EMPTY;
	mApp;
	const char* cprauth = proofed_authstring.c_str();
	app->redisCommand("hget %s %s", __NODE_COLLISION_TOK_LOOKUP, cprauth);
	return Redis::toString(app->reply());
}

inline std::string proofed_authstring(std::string e_username)
{
	if (!hashkey_exists(__NODE_COLLISION_STR_LOOKUP, e_username))
		return EMPTY;
	mApp;
	const char* cenc_id = e_username.c_str();
	app->redisCommand("hget % %s", __NODE_COLLISION_STR_LOOKUP, cenc_id);
	return Redis::toString(app->reply());
}

inline std::string access_token(std::string proofed_authstring)
{
	if (!hashkey_exists(__NODE_AUTH_LOOKUP, proofed_authstring)) return EMPTY;
	mApp;
	const char* cprauth = proofed_authstring.c_str();
	app->redisCommand("hget %s %s", __NODE_AUTH_LOOKUP, cprauth);
	return Redis::toString(app->reply());
}


}//namespace Lookups
}//namespace Sessions
#endif /* LOOKUPS_H_ */
