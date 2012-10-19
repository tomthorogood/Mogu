/*
 * SessionHandler.cpp
 *
 *  Created on: Oct 19, 2012
 *      Author: tom
 */

#include <Sessions/SessionHandler.h>
#include <Exceptions/Exceptions.h>
#include <Types/RedisStorageRequest.h>
#include <Mogu.h>
#include <Redis/RedisCore.h>

namespace Sessions{

inline size_t max_iterations()
{
	mApp;
	app->redisCommand("dbsize");
	return Redis::getInt(app->reply());
}

inline std::string build_session_node(const std::string& sessionid)
{
	return "s."+sessionid;
}

inline std::string get_previous_session_id(const std::string& sessionid)
{
	mApp;
	std::string metanode = build_session_node(sessionid)+"."+__META_HASH;
	app->redisCommand("hget", metanode, __PREV_HASH);
	return Redis::toString(app->reply());
}

SessionHandler::SessionHandler(const std::string& session_id)
:	__linked_sessions()
{
	__linked_sessions[0] = session_id;
	bool iterated = false;
	while (!iterated)
	{
		try
		{
			gather_linked_sessions();
			iterated = true;
		}
		catch (const Exceptions::Err_CorruptedSessionList& e)
		{
			mApp;
			app->log("warning") << e.what();
			app->log("warning") << "Pointing " << e.getSessionID() << " to global";
			std::string node = build_session_node(
					e.getSessionID())+"."+__META_HASH;
			app->redisCommand("hset", node,__PREV_HASH,"global");
		}
	}

}

void SessionHandler::gather_linked_sessions()
{
	std::string current = __linked_sessions[0];
	size_t iter = 0;
	size_t max = max_iterations();

	while (current != "global" && iter < max)
	{
		std::string next = get_previous_session_id(current);
		if (next == current) throw Exceptions::Err_CorruptedSessionList
				("Linked Session List Corruption: Session References Self"
				, next);
		__linked_sessions[++iter] = next;
		current = next;
	}
	if (iter >= max)
	{
		throw Exceptions::Err_CorruptedSessionList(
				"Linked Session List Corruption: List larger than database?",
				current);
	}
}

bool SessionHandler::store(Redis::RedisStorageRequest& request)
{
	request.setSessionID(__linked_sessions[0]);
	return request.execute();
}

Nodes::NodeValue SessionHandler::retrieve(const std::string& plaintext_node)
{
	Nodes::NodeValue v;
	Parsers::NodeValueParser(plaintext_node, &v);
	return v;
}

}//namespace Sessions

