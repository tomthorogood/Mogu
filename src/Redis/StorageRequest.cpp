/*
 * RedisStorageRequest.cpp
 *
 *  Created on: Oct 19, 2012
 *      Author: tom
 */

#include <Redis/StorageRequest.h>
#include <Redis/StoragePolicyLookup.h>
#include <Parsers/StyleParser.h>
#include <Security/Security.h>
#include <hash.h>
#include <Mogu.h>

inline std::string build_policy_node(const std::string& name)
{
	return "policies."+name;
}

namespace Redis
{

StorageRequest::StorageRequest(
		std::string& __destination
		,NodeValue& input)
: destination(__destination)
, tokenizer(destination)
, value(input)
{
	__init__();
	mApp;
	session_id = app->sessionID();
	build_command();

}
StorageRequest::~StorageRequest  () { delete lookup;}

void StorageRequest::__init__()
{
	if (!policyLookup())
	{
		//TODO THROW ERROR -- policy does not exist!
	}

}
bool StorageRequest::policyLookup()
{
	using namespace Parsers;
	policy_token = tokenizer.next();
	policy_token = policy_token.substr(1,policy_token.length()-2);

	lookup = new StoragePolicyLookup(policy_token);
	return (lookup->policyExists());
}

void StorageRequest::build_command()
{
	mApp;
	using namespace Enums::SubmissionPolicies;
	//All other entities must first have been set.

	//TODO add ability to lpush or rpush
	StorageType nodetype = lookup->getStorageType();
	switch(nodetype)
	{
	case Enums::SubmissionPolicies::string:
		command = "set %s";
		break;
	case list:
		command = "rpush %s %s";
		break;
	case hash:
		command = "hset %s %s %s";
		break;
	case set:
		command = "sadd %s %s";
		break;
	}

	node_body = "s."+session_id+"."+Hash::toHash(policy_token);

	if (tokenizer.hasNext())
	{
		NodeValue varg;
		app->interpreter().giveInput(tokenizer.next(), varg);
		if (varg.getType() == int_value)
		{
			varg.setString(itoa(varg.getInt()));
		}
		arg = varg.getString();
	}

	//TODO add support for floating-point-to-string
	switch(value.getType())
	{
	case int_value:
		f_value = itoa(value.getInt());
		break;
	default:
		f_value = value.getString();
	}

	if (lookup->isEncrypted()) f_value = Security::encrypt(f_value);
}

bool StorageRequest::execute()
{
	mApp;
	const char* cbody = node_body.c_str();
	const char* carg = arg.c_str();
	const char* cval = f_value.c_str();

	app->redisCommand(command.c_str(), cbody, carg, cval);
	app->freeReply();
	return true;
}

}//namespace Redis

