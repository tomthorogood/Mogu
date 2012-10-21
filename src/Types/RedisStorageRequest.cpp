/*
 * RedisStorageRequest.cpp
 *
 *  Created on: Oct 19, 2012
 *      Author: tom
 */

#include <Types/RedisStorageRequest.h>
#include <Parsers/StyleParser.h>
#include <hash.h>
#include <Mogu.h>

inline std::string build_policy_node(const std::string& name)
{
	return "policies."+name;
}

namespace Redis
{

static std::string global_session = "global";

RedisStorageRequest::RedisStorageRequest(std::string nodeBody)
: session_id(global_session)
{
	node_body = nodeBody;
	encrypted 	= false;
	value 		= 0;
	ready_state = bit_node_body;
	mode 		= Enums::SubmissionPolicies::replace;
	dtype 		= Enums::SubmissionPolicies::none;
	nodetype	= Enums::SubmissionPolicies::string;
	field		= EMPTY;
}

bool RedisStorageRequest::policyLookup(const std::string& storageName)
{
	mApp;
	using namespace Parsers;
	std::string policy = build_policy_node(storageName);
	app->redisCommand("exists", policy);
	if ((bool) Redis::getInt(app->reply()))
	{
		Nodes::NodeValue v;
		std::string str;
		if (StyleParser::nodeHasField(policy, "encrypted"))
		{
			str = StyleParser::getWidgetField(policy, "encrypted");
			Parsers::NodeValueParser p(str,&v);
			encrypted = (bool) v.getInt();
		}

		if (StyleParser::nodeHasField(policy, "mode"))
		{
			str = StyleParser::getWidgetField(policy, "mode");
			Parsers::NodeValueParser p(str,&v, NULL,
					&Parsers::enum_callback <Parsers::StorageModeParser>);
			mode = (Enums::SubmissionPolicies::StorageMode) v.getInt();
		}
		ready_state |= bit_mode;

		if (StyleParser::nodeHasField(policy, "data_type"))
		{
			str = StyleParser::getWidgetField(policy, "data_type");
			Parsers::NodeValueParser p(str, &v, NULL,
					&Parsers::enum_callback <Parsers::StorageWrappingParser>);
			dtype = (Enums::SubmissionPolicies::DataWrapping) v.getInt();
		}
		ready_state |= bit_dtype;

		if (StyleParser::nodeHasField(policy, "storage_type"))
		{
			str = StyleParser::getWidgetField(policy, "storage_type");
			Parsers::NodeValueParser p(str, &v, NULL,
					&Parsers::enum_callback <Parsers::StorageTypeParser>);
			nodetype = (Enums::SubmissionPolicies::StorageType) v.getInt();
		}
		ready_state |= bit_nodetype;

		if (StyleParser::nodeHasField(policy, "node"))
		{
			str = StyleParser::getWidgetField(policy, "node");
			Parsers::NodeValueParser p(str, &v);
			node_body = v.getString();
		}

		if (StyleParser::nodeHasField(policy, "field"))
		{
			str = StyleParser::getWidgetField(policy, "field");
			Parsers::NodeValueParser p(str,&v);
			field = v.getString();
		}

		return true;
	}
	else return false;
}

void RedisStorageRequest::build_command()
{
	using namespace Enums::SubmissionPolicies;

	//All other entities must first have been set.
	if (ready_state | bit_command == 0x7F)
	{
		std::string command = "";
		switch(nodetype)
		{
		case string:{
			command += "set";
			break;}
		case list:{
			command += "rpush";
			break;
		}
		case hash:{
			command += "hset";
			break;
		}
		case set:{
			command += "sadd";
			break;
		}
		}

		command += " s."+ session_id + "." + Hash::toHash(node_body);
		if (nodetype == hash) command += " " + field;

		// Note: Should already have been parsed!
		command += " " + value->getString();

		ready_state |= bit_command;
	}
}

bool RedisStorageRequest::execute()
{
	if (!ready()) return false;
	mApp;
	app->redisCommand(command);
	app->freeReply();
	return true;
}

}//namespace Redis


