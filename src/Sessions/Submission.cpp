/*
 * Submission.cpp
 *
 *  Created on: Aug 1, 2012
 *      Author: tom
 */

#include <Sessions/Submission.h>
#include <Sessions/Lookups.h>
#include <Static.h>
#include <Redis/RedisCore.h>
#include <Parsers/NodeValueParser.h>
#include <crypt/BlowfishKey.h>
#include <crypt/PacketCenter.h>
#include <Wt/WFormWidget>
#include <Wt/WLineEdit>
#include <Types/Enums.h>
#include <Types/NodeValue.h>
#include <Parsers/Parsers.h>
#include <Mogu.h>
#include <crypt/PacketCenter.h>
#include <sstream>
#include <Security/Security.h>
#include <Wt/WText>
#include <TurnLeftLib/Utils/inlines.h>

namespace Sessions{
namespace SubmissionHandler{
using namespace Enums::SubmissionPolicies;
using namespace Sessions::Lookups;
using TurnLeft::Utils::trimchar;

inline bool session_widget_exists(std::string node)
{
	mApp;
	app->redisCommand("exists", node);
	return (bool) Redis::getInt(app->reply());
}

void absorb(std::string value_unenc, std::string snode)
{


	namespace Widget = Enums::WidgetTypes;
	/* We will only continue if all authorization checks are passed.
	 */
	mApp;
	std::string session_id = app->sessionID();

	/* Determine the location of the node's template */
	std::string storage_locker = Hash::toHash(snode);

	/* The storage node will be a hashed version of the node template name,
	 * within the current session namespace.
	 */
	std::string storageNode = session_node(session_id, snode);

	/* If widget info is stored at widgets.firstName,
	 * and the user session id is 235r308gg, and the
	 * hashed name of "firstName" is 123hu, the storage node would be:
	 * s.235r308gg.123hu
	 */
	std::string value_enc;
	bool encrypted = requiresEncryption(snode);
	if (encrypted)
	{
		value_enc = Security::encrypt(value_unenc);
	}

	StorageMode mode_ = getStorageMode(snode);
	StorageType type_ = getStorageType(snode);
	DataWrapping wrap_ = getDataWrapping(snode);
	std::string redis_command = "";

	/* Encrypted data will always be interpreted as strings,
	 * so we don't need to wrap it.
	 */
	if (!encrypted)
	{
		switch(wrap_)
		{
		case enumerated_type:{
			value_unenc = "{"+value_unenc+"}";
			break;}
		case static_node:{
			value_unenc = "%"+value_unenc+"%";
			break;}
		case dynamic_node:{
			value_unenc = "@"+value_unenc+"@";
			break;}
		case integral_type:{
			value_unenc = "^"+value_unenc+"^";
			break;}
		case floating_type:{
			value_unenc = "~"+value_unenc+"~";
			break;}
		case file:{
			value_unenc = "`"+value_unenc+"`";
			break;}
		default:break;
		}
	}
	switch(type_)
	{

	/* If a list has 'replace' mode set, we must first delete
	 * the current list. The command remains the same, however.
	 */
	case list:{
		if (mode_ == replace)
		{	mApp;
			app->redisCommand("del", storageNode);
		}
		redis_command = "rpush";
		break;}

	/* If a hash has the append mode set, and the node is encrypted,
	 * we must first decrypt the current value, append the unencrypted
	 * new value, and then encrypt them both together. Otherwise, bad
	 * things would happen when trying to decrypt all the data.
	 */
	case hash:{
		std::string field = getHashField(snode);
		if (mode_ == append)
		{	mApp;
			app->redisCommand("hget", storageNode,field);
			std::string current_val = Redis::toString(app->reply());
			if (encrypted)
			{
				current_val = Security::decrypt(current_val);
				std::string concat_val = current_val + " " + value_unenc;
				value_enc = Security::encrypt(concat_val);
			}
			else
			{
				value_unenc = current_val + " " + value_unenc;
			}
		}
		redis_command = "hset";
		break;}

	/* String values can be appended with a standard redis command if
	 * not encrypted. Otherwise, though, we must follow the same procedure
	 * of decryption, concatenation, encryption as above.
	 */
	case Enums::SubmissionPolicies::string:{
		if (mode_ == append)
		{
			redis_command = "append";
			if (encrypted)
			{
				mApp;
				app->redisCommand("get", storageNode);
				std::string current_val = Redis::toString(app->reply());
				current_val = Security::decrypt(current_val);
				std::string concat_val = current_val + " " + value_unenc;
				value_enc = Security::encrypt(concat_val);
			}
			value_enc = " " + value_enc;
		}
		else
		{
			redis_command = "set";
		}
		break;}

	default: break;
	}
	std::string value = (encrypted) ? value_enc : value_unenc;

	// "command" -> "command node"
	redis_command = redis_command + " " + storageNode;
	if (type_==hash)
	{
		//"command node" -> "command node field"
		redis_command = redis_command + " " + getHashField(snode);
	}
	//"command node" | "command node field" ->
	//				"command node "value"" | "command node field "value" "
	app->redisCommand(redis_command, value);
	app->freeReply();
}

/*Returns the session ID in this user's chain where the node is found first. */
std::string node_session(std::string session, std::string ph_node)
{
	if ( session_widget_exists(prhshd_session_node(session,ph_node))
		|| session == "global") return session;
	return node_session(linked_session(session),ph_node);
}

std::string storage_arg(std::string pt_node_name)
{
	std::string node = "widgets."+pt_node_name+".policy";
	if (!hashkey_exists(node, "arg")) return EMPTY;
	mApp;
	app->redisCommand("hget", node, "arg");
	return Redis::toString(app->reply());
}

bool requiresEncryption(const std::string& snode)
{
	std::string nodePolicy = "widgets."+snode+".policy";
	if (!hashkey_exists( nodePolicy, "encrypted")) return false;
	mApp;
	app->redisCommand("hget", nodePolicy, "encrypted");
	Nodes::NodeValue val;
	Parsers::NodeValueParser parser(Redis::toString(app->reply()), &val);

	return (bool) val.getInt();
}

StorageMode getStorageMode(const std::string& snode)
{
	std::string nodePolicy = "widgets."+snode+".policy";
	mApp;
		app->redisCommand("hget", nodePolicy,"mode");
	Nodes::NodeValue val;
	Parsers::NodeValueParser parser(
			Redis::toString(app->reply())
			,&val, 0x0 //No need to pass in a widget
			,&Parsers::enum_callback <Parsers::StorageModeParser>);
	return (StorageMode) val.getInt();
}

StorageType getStorageType(const std::string& snode)
{
	std::string nodePolicy = "widgets."+snode+".policy";
	mApp;
	app->redisCommand("hget", nodePolicy, "storage_type");
	Nodes::NodeValue val;
	Parsers::NodeValueParser parser(
			Redis::toString(app->reply())
			,&val
			,0x0 // No need to pass in a widget
			,&Parsers::enum_callback <Parsers::StorageTypeParser>);
	return (StorageType) val.getInt();
}

DataWrapping getDataWrapping(const std::string& snode)
{
	std::string nodePolicy = "widgets."+snode+".policy";
	mApp;
	app->redisCommand("hget", nodePolicy, "data_type");
	Nodes::NodeValue val;
	Parsers::NodeValueParser parser(
			Redis::toString(app->reply()),
			&val,
			0x0,
			&Parsers::enum_callback <Parsers::StorageWrappingParser>);
	return (DataWrapping) val.getInt();
}

std::string getHashField(const std::string& snode)
{
	std::string nodePolicy = "widgets."+snode+".policy";
	mApp;
	app->redisCommand("hget", nodePolicy, "field");
	return Redis::toString(app->reply());
}

std::string getSlotName(const std::string& snode)
{
	std::string nodePolicy = "widgets."+snode+".policy";
	mApp;
	app->redisCommand("hget", nodePolicy, "slot");
	return Redis::toString(app->reply());
}

std::string userNodeLookup(
		std::string sessionid,
		std::string storage_name,
		std::string arg)
{
	using std::string;
	using namespace Enums::NodeValueTypes::RedisTypes;
	// The hashed value of the storage node's name
	string h_st_node = Hash::toHash(storage_name);
	string node =
			prhshd_session_node(
					node_session(sessionid,h_st_node),h_st_node);

	/* Determine whether the data in that node was supposed to have been encrypted */
	bool encrypted = requiresEncryption(storage_name);

	/* Determine the type of node that we're dealing with */
	mApp;
	app->redisCommand("type", node);
	string node_type = Redis::toString(app->reply());

	/* If we have a list or hash node, get the extra value that should
	 * have been passed in.
	 */
	if (node_type == REDIS_HSH)
	{
		// If we can't parse down to the final value, return the node itself
		if (arg == EMPTY) return node;
		mApp;
		app->redisCommand("hget",node,arg);
	}
	else if (node_type == REDIS_LST)
	{
		// If we can't parse down to the final value, return the node itself
		if (arg == EMPTY) return node;
		mApp;
		app->redisCommand("lindex",node,arg);
	}
	else if (node_type == REDIS_STR)
	{
		mApp;
		app->redisCommand("get",node);
	}
	else return EMPTY;

	string raw_string = Redis::toString(app->reply());
	if (encrypted)
	{
		std::string dec = Security::decrypt(raw_string);
		char padding = '_';
		trimchar(dec,padding);
		return dec;
	}
	return raw_string;
}

std::string dynamicLookup(std::string storage_name, std::string arg)
{
	using namespace Application;
	using namespace Enums::NodeValueTypes::RedisTypes;
	using std::string;
	mApp;
	string sessionid = app->sessionID();
	return userNodeLookup(sessionid, storage_name, arg);
}

}//namespace SubmissoinHandler
}//namespace Sessions


