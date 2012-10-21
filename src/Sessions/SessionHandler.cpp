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
#include <Types/NodeValue.h>
#include <Parsers/NodeValueParser.h>
#include <hash.h>

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

Nodes::NodeValue SessionHandler::retrieve(
		const std::string& plaintext_node, const std::string& arg)
{
	mApp;
	std::string node;
	std::string node_val;
	std::string node_type;

	Nodes::NodeValue v;
	size_t iter;
	size_t max = __linked_sessions.size();

	bool found;


	for (iter =0, found=false; (iter < max && found==false); ++iter)
	{
		node = build_session_node(__linked_sessions[iter]);
		node += "." + Hash::toHash(plaintext_node);
		app->redisCommand("exists", node);
		found = (bool) Redis::getInt(app->reply());
		if (found) break;
	}

	if (!found) return v;

	app->redisCommand("type", node);
	node_type = Redis::toString(app->reply());


	using namespace Enums::NodeValueTypes::RedisTypes;
	if (node_type == REDIS_HSH)
	{
		app->redisCommand("hget", node, arg);
	}
	else if (node_type == REDIS_LST)
	{
		app->redisCommand("lindex", node, arg);
	}
	else
	{
		app->redisCommand("get", node);
	}
	node_val = Redis::toString(app->reply());

	Parsers::NodeValueParser p(node_val, &v);
	return v;
}

}//namespace Sessions

