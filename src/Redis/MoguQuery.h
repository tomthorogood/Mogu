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
	}

	inline bool node_exists(const std::string& node_name, Keyspace k,
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

	inline std::string getSessionAuthToken(const std::string& session)
	{
		return getSessionMetaValue(session, __AUTH_HASH);
	}

private:
	redisContext* redis;
};

}


#endif /* MOGUQUERY_H_ */
