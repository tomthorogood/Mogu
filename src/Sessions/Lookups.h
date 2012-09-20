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

inline bool auth_token_exists(std::string token)
{
	Redis::command("sismember", __NODE_ALL_AUTHS, token);
	return (bool) Redis::getInt();
}

inline bool auth_string_exists(std::string str)
{
	Redis::command("hexists", __NODE_AUTH_LOOKUP, str);
	return (bool) Redis::getInt();
}

inline bool session_exists(std::string sessionid)
{
	std::string checknode = prhshd_session_node(sessionid, __META_HASH);
	Redis::command("exists", checknode);
	return (bool) Redis::getInt();
}

inline bool hashkey_exists(std::string node, std::string field)
{
	Redis::command("hexists", node, field);
	return (bool) Redis::getInt();
}

inline std::string last_session(std::string plain_userid)
{
	std::string e_userid = Security::encrypt(plain_userid);
	if (!hashkey_exists(__NODE_SESSION_LOOKUP, e_userid)) return EMPTY;

	Redis::command("hget", __NODE_SESSION_LOOKUP, e_userid);
	return Redis::toString();
}

inline std::string user_salt(std::string e_userid)
{
	Redis::command("hget", __NODE_SALT_LOOKUP, e_userid);
	return Redis::toString();
}

inline std::string linked_session(std::string sessionid)
{
	std::string meta_node = prhshd_session_node(sessionid, __META_HASH);
	if (!hashkey_exists(meta_node, __PREV_HASH)) return EMPTY;
	Redis::command("hget", meta_node, __PREV_HASH);
	return Redis::toString();
}

inline std::string raw_last_authtoken(std::string session)
{
	std::string meta_node = session_node(session, "meta");
	Redis::command("get", meta_node, __AUTH_HASH);
	return Redis::toString();
}

inline std::string proofed_last_authtoken(std::string proofed_authstring)
{
	if (!hashkey_exists(__NODE_COLLISION_TOK_LOOKUP,proofed_authstring))
		return EMPTY;
	Redis::command("hget", __NODE_COLLISION_TOK_LOOKUP, proofed_authstring);
	return Redis::toString();
}

inline std::string proofed_authstring(std::string e_username)
{
	if (!hashkey_exists(__NODE_COLLISION_STR_LOOKUP, e_username))
		return EMPTY;
	Redis::command("hget", __NODE_COLLISION_STR_LOOKUP, e_username);
	return Redis::toString();
}

inline std::string access_token(std::string proofed_authstring)
{
	if (!hashkey_exists(__NODE_AUTH_LOOKUP, proofed_authstring)) return EMPTY;
	Redis::command("hget", __NODE_AUTH_LOOKUP, proofed_authstring);
	return Redis::toString();
}


}//namespace Lookups
}//namespace Sessions
#endif /* LOOKUPS_H_ */
