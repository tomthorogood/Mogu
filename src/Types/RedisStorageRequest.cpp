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

RedisStorageRequest::RedisStorageRequest(std::string policyName)
: session_id(global_session)
{
	policy_name = policyName;
	__init__();
}

RedisStorageRequest::RedisStorageRequest(
		std::string policyName
		,const std::string& sessionID
		,Nodes::NodeValue& input)
: session_id(global_session)
{
	policy_name = policyName;
	__init__();
	setSessionID(sessionID);
	setInput(&input);
	build_command();

}

void RedisStorageRequest::__init__()
{
	encrypted = false;
	value = 0;
	ready_state = bit_node_body;
	mode = Enums::SubmissionPolicies::replace;
	dtype = Enums::SubmissionPolicies::none;
	nodetype = Enums::SubmissionPolicies::string;
	field = EMPTY;
	policyLookup();

}
bool RedisStorageRequest::policyLookup()
{
	mApp;
	using namespace Parsers;
	std::string policy = build_policy_node(policy_name);
	app->redisCommand("exists", policy);
	if ((bool) Redis::getInt(app->reply()))
	{
		Nodes::NodeValue v;
		std::string str;
		if (StyleParser::nodeHasField(policy, "encrypted"))
		{
			str = StyleParser::getWidgetField(policy, "encrypted");
			Parsers::NodeValueParser p(str,v);
			encrypted = (bool) v.getInt();
		}

		if (StyleParser::nodeHasField(policy, "mode"))
		{
			str = StyleParser::getWidgetField(policy, "mode");
			Parsers::NodeValueParser p(str,v, NULL,
					&Parsers::enum_callback <Parsers::StorageModeParser>);
			mode = (Enums::SubmissionPolicies::StorageMode) v.getInt();
		}
		ready_state |= bit_mode;

		if (StyleParser::nodeHasField(policy, "data_type"))
		{
			str = StyleParser::getWidgetField(policy, "data_type");
			Parsers::NodeValueParser p(str, v, NULL,
					&Parsers::enum_callback <Parsers::StorageWrappingParser>);
			dtype = (Enums::SubmissionPolicies::DataWrapping) v.getInt();
		}
		ready_state |= bit_dtype;

		if (StyleParser::nodeHasField(policy, "storage_type"))
		{
			str = StyleParser::getWidgetField(policy, "storage_type");
			Parsers::NodeValueParser p(str, v, NULL,
					&Parsers::enum_callback <Parsers::StorageTypeParser>);
			nodetype = (Enums::SubmissionPolicies::StorageType) v.getInt();
		}
		ready_state |= bit_nodetype;

		return true;
	}
	else return false;
}

void RedisStorageRequest::build_command()
{
	using namespace Enums::SubmissionPolicies;

	//All other entities must first have been set.
	if ( (ready_state | bit_command) == 0x7F)
	{
		//TODO add ability to lpush or rpush
		std::string command = "";
		switch(nodetype)
		{
		case string:
			command += "set";
			break;
		case list:
			command += "rpush";
			break;
		case hash:
			command += "hset";
			break;
		case set:
			command += "sadd";
			break;
		}

		command += " s."+ session_id + "." + Hash::toHash(policy_name);
		if (nodetype == hash) command += " " + field + " \"";

		//TODO add support for floating-point-to-string
		switch(value->getType())
		{
		case Nodes::int_value:
			command += itoa(value->getInt());
			break;
		default:
			command += value->getString();
		}
		command += "\"";

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

