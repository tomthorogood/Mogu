/*
 * SessionValueRequest.cpp
 *
 *  Created on: Oct 28, 2012
 *      Author: tom
 */

#include <declarations.h>
#include <Mogu.h>
#include <Redis/RedisCore.h>
#include <hash.h>
#include <Redis/StoragePolicyLookup.h>
#include <Security/Encryption.h>
#include <crypt/PacketCenter.h>

#include <Redis/SessionValueRequest.h>

namespace Redis{

using namespace Enums::SubmissionPolicies;


SessionValueRequest::SessionValueRequest(
		std::vector <std::string>& session_ids
		,const std::string& key
		,std::string arg)
: lookup(key)
{
	process_request(session_ids, key, arg);
}

SessionValueRequest::SessionValueRequest (
		const std::string& key, std::string arg)
	:lookup(key)
{
	mApp;
	keyhash = Hash::toHash(key);
	node_name = EMPTY;
	retrieved_value = EMPTY;

	size_t max_iterations;
	app->redisCommand("dbsize");
	max_iterations = (size_t) Redis::getInt(app->reply());

	std::string session_iter = app->sessionID();
	size_t num_iter = 0;

	std::vector <std::string> session_ids;
	session_ids.push_back(session_iter);
	while (session_iter != "global" && num_iter < max_iterations)
	{
		++max_iterations;
		std::string tmp_node = build_session_node(session_iter, __META_HASH);
		const char* csession = tmp_node.c_str();
		app->redisCommand("hget %s %s"
				,csession
				,__PREV_HASH);
		if (app->reply()->type == REDIS_REPLY_STRING)
		{
			session_iter = Redis::toString(app->reply());
		}
		else session_iter = "global";

		session_ids.push_back(session_iter);
	}

	process_request(session_ids, keyhash, arg);
}

void SessionValueRequest::process_request(
		std::vector<std::string>& session_ids
		,const std::string& key
		,const std::string& arg)
{
	mApp;
	size_t max_iterations = session_ids.size();
	std::string nodename;
	for (size_t i = 0; i < max_iterations; ++i)
	{
		std::string session_iter = session_ids[i];
		nodename = build_session_node(session_iter, key);
		const char* cnode = nodename.c_str();
		app->redisCommand("exists %s", cnode);
		if ( (bool) Redis::getInt(app->reply()) )
		{
			if (arg != EMPTY)
			{
				const char* carg = arg.c_str();
				if (lookup.getStorageType() == hash)
				{
					app->redisCommand("hexists %s %s", cnode, carg);
					if ( Redis::getBool(app->reply()) ) break;
				}
				else if (lookup.getStorageType() == list)
				{
					app->redisCommand("llen %s", cnode);
					if ( Redis::getInt(app->reply()) > atoi(carg) )
						break;
				}
				else if (lookup.getStorageType() == set)
				{
					app->redisCommand("sismember %s", cnode);
					if (Redis::getBool(app->reply())) break;
				}
			}
			else if (lookup.getStorageType() != Enums::SubmissionPolicies::string)
			{
				//TODO THROW ERROR
			}
			else break;
		}
	}

	node_name = nodename;
	const char* cnode = node_name.c_str();
	const char* carg = arg.c_str();

	switch(lookup.getStorageType())
	{
	case Enums::SubmissionPolicies::string:
		app->redisCommand("get %s", cnode);
		break;
	case hash:
		app->redisCommand("hget %s %s", cnode, carg);
		break;
	case list:
		app->redisCommand("lindex %s %d", cnode, atoi(carg));
		break;
	default: break;
	}
	retrieved_value = Redis::toString(app->reply());
	if (lookup.isEncrypted())
		retrieved_value = Security::decrypt(
				retrieved_value, NO_TRANSLATION);

}

}//namespace Redis
