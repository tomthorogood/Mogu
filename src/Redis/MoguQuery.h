/*
 * MoguQuery.h
 *
 *  Created on: Oct 21, 2012
 *      Author: tom
 */

#ifndef MOGUQUERY_H_
#define MOGUQUERY_H_

#include <declarations.h>
#include <Redis/RedisCore.h>
#include <Exceptions/Exceptions.h>
#include <Sessions/Lookups.h>


namespace Redis{


/*!\brief Displays information about the Mogu keyspaces of a Redis database.
 *
 * This class abstracts common queries about the database in order to keep
 * global changes to a minimum and establish a safe syntax to be used throughout
 * Mogu development.
 */

enum Keyspace {
	widgets,
	sessions,
	policies,
	global_events,
	meta,
	data
};


static std::map <Keyspace, std::string> keyspaceString
	= create_map <Keyspace, std::string>
	(widgets, "widgets")
	(sessions, "s")
	(policies, "policies")
	(global_events, "events")
	(meta, "meta")
	(data, "data")
;

class MoguQuery
{
public:
	MoguQuery(redisContext* r)
	{
		redis = r;
		userid = EMPTY;
		__wasEncrypted = false;
		e_userid = EMPTY;
	}

	inline bool exists(const std::string& node_name, Keyspace k,
			const std::string& suffix=EMPTY)
	{
		std::string key = keyspaceString[k] + "." + node_name;
		if (suffix != EMPTY) key += "." + suffix;
		void* vreply = Redis::command(redis, "exists", key);
		redisReply* reply = (redisReply*) vreply;
		bool e = (bool) reply->integer;
		freeReplyObject(reply);
		return e;
	}

	inline bool session_exists (const std::string& sessionID)
	{
		void* vr = Redis::command(
				redis, "sismember", __NODE_SESSION_SET, sessionID);
		redisReply* r = (redisReply*) vr;
		bool e = (bool) r->integer;
		freeReplyObject(r);
		return e;
	}

	inline bool user_exists()
	{
		std::vector <std::string> userList;
		void* vr = Redis::command(redis, "hkeys", __NODE_SESSION_LOOKUP);
		redisReply* r = (redisReply*) vr;
		Redis::toVector(r,userList);
		return
				(std::find(userList.begin(),userList.end(), encryptedID())
			!= userList.end());
	}

	inline std::string getSessionMetaValue(
			const std::string& session,const std::string& key)
	{
		std::string session_node = "s."+session+"."+__META_HASH;
		void* vr = Redis::command(redis, "hget", session_node, key);
		redisReply* r = (redisReply*) vr;
		std::string value = r->str;
		freeReplyObject(r);
		return value;
	}

	inline std::string getSalt()
	{
		using Exceptions::Err_NoUserSet;
		if (userid == EMPTY)
			throw Err_NoUserSet("Attempt to get user data, but no user is set.");
		void* v = Redis::command(redis, "hget", __NODE_SALT_LOOKUP, userid);
		redisReply* r = (redisReply*) v;
		std::string salt = r->str;
		freeReplyObject(r);
		return salt;
	}

	inline void setUser(const std::string& user)
	{
		userid = user;
		TurnLeft::Utils::stolower(userid);
	}

	/*!\brief Returns what the auth string is for the information provided
	 * by the user when they attempt to login.
	 * @param plain_userAuth
	 * @return The generated auth string. This does not return the pre-existing
	 * auth string that was created upon account creation.
	 */
	inline std::string getAttemptedAuthString(const std::string& plain_userAuth)
	{
		/* Create what the user's raw auth string should be. */
		std::string e_raw_auth_string = Security::create_raw_auth_string
				(userid, getSalt(), plain_userAuth);

		/* Determine whether the user's auth string was collision proofed
		 * upon creation. If so, bring the should-be string through the
		 * same number of cycles.
		 */
		std::string auth_string = e_raw_auth_string;
		int proofed_cycles = stoi(
				Sessions::Lookups::proofed_authstring(encryptedID()));
		while (proofed_cycles > 0)
		{
			auth_string = Security::collision_proof(auth_string);
			--proofed_cycles;
		}

		return auth_string;
	}

	inline std::string getUserLastSession()
	{
		redisReply* r;
		void* v = Redis::command(
				redis, "hget", __NODE_SESSION_LOOKUP, encryptedID());
		r = (redisReply*) v;
		std::string last_session = r->str;
		freeReplyObject(r);
		return last_session;
	}

	inline std::string getSessionAuthToken(const std::string& session)
	{
		return getSessionMetaValue(session, __AUTH_HASH);
	}

	bool matchesUserAuth(const std::string& userauth_in)
	{
		return Sessions::Lookups::hashkey_exists(
				__NODE_AUTH_LOOKUP, userauth_in);
	}


private:
	redisContext* redis;
	std::string userid;
	std::string e_userid;
	bool __wasEncrypted;

	inline std::string encryptedID()
	{
		if (!__wasEncrypted)
		{
			e_userid = Security::encrypt(userid);
			__wasEncrypted = true;
		}
		return e_userid;
	}
};

}


#endif /* MOGUQUERY_H_ */
