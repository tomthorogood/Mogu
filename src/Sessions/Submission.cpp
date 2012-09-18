/*
 * Submission.cpp
 *
 *  Created on: Aug 1, 2012
 *      Author: tom
 */

#include <Sessions/Submission.h>
#include <hash.h>
#include <Core/Dynamic.h>
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

namespace Sessions{
namespace SubmissionHandler{
using Goo::Dynamic;
using namespace Enums::SubmissionPolicies;


inline std::string session_widget(std::string session, std::string p_storage)
{
	return "s."+session+"."+Hash::toHash(p_storage);
}

inline std::string getPrevSession(std::string session)
{
	Redis::command(
			"hget",
			"s."+session+"."+Hash::toHash("meta"),
			Hash::toHash("prev"));
	return Redis::toString();
}

inline bool session_widget_exists(std::string node)
{
	Redis::command("exists", node);
	return (bool) Redis::getInt();
}

void absorb(std::string value_unenc, std::string snode)
{


	namespace Widget = Enums::WidgetTypes;
	/* We will only continue if all authorization checks are passed.
	 */
	std::string session_id =
			Application::requestSessionID(
					Application::mogu()->sessionId());

	/* Determine the location of the node's template */
	std::string storage_locker = Hash::toHash(snode);

	/* The storage node will be a hashed version of the node template name,
	 * within the current session namespace.
	 */
	std::string storageNode = "s."+session_id+"."+storage_locker;

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
		{
			Redis::command("del", storageNode);
			Redis::clear();
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
		{
			Redis::command("hget", storageNode,field);
			std::string current_val = Redis::toString();
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
				Redis::command("get", storageNode);
				std::string current_val = Redis::toString();
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
	Redis::command(redis_command, value);
	Redis::clear();
}

/*Returns the session ID in this user's chain where the node is found first. */
std::string node_session(std::string session, std::string node)
{
	if (
			session_widget_exists(session_widget(session,node))
			|| session == "global") return session;
	return node_session(getPrevSession(session),node);
}

void emerge(Dynamic* widget)
{

	using namespace Application;
	using namespace Enums::WidgetTypes;
	using std::string;

	string
		sessionid
		,prev_session
		,hprev
		,storage
		,swidget
	;

	hprev = Hash::toHash("prev");
	storage = widget->storageLocker();
	sessionid = requestSessionID(Application::mogu()->sessionId());
	if (sessionid == "global") return;

	swidget = session_widget(node_session(sessionid, storage),storage);

	WidgetTypes type = (WidgetTypes)
			(widget->getProperties()->type & WIDGET_HO_BITS);

	switch(type)
	{
	case text:{
		Redis::command("get", swidget);
		std::string _newtext = Redis::toString();

		if (requiresEncryption(storage))
		{
			_newtext = Security::decrypt(_newtext);
		}

		Wt::WString newtext(_newtext);
		Wt::WText* w = (Wt::WText*) widget->widget(0);
		w->setText(newtext);
		break;}
	default: return;
	}

}

bool requiresEncryption(const std::string& snode)
{
	std::string nodePolicy = "widgets."+snode+".policy";
	Redis::command("hget", nodePolicy, "encrypted");
	Nodes::NodeValue val;
	Parsers::NodeValueParser parser(Redis::toString(), &val);

	return (bool) val.getInt();
}

StorageMode getStorageMode(const std::string& snode)
{
	std::string nodePolicy = "widgets."+snode+".policy";
	Redis::command("hget", nodePolicy,"mode");
	Nodes::NodeValue val;
	Parsers::NodeValueParser parser(
			Redis::toString()
			,&val, 0x0 //No need to pass in a widget
			,&Parsers::enum_callback <Parsers::StorageModeParser>);
	return (StorageMode) val.getInt();
}

StorageType getStorageType(const std::string& snode)
{
	std::string nodePolicy = "widgets."+snode+".policy";
	Redis::command("hget", nodePolicy, "storage_type");
	Nodes::NodeValue val;
	Parsers::NodeValueParser parser(
			Redis::toString()
			,&val
			,0x0 // No need to pass in a widget
			,&Parsers::enum_callback <Parsers::StorageTypeParser>);
	return (StorageType) val.getInt();
}

DataWrapping getDataWrapping(const std::string& snode)
{
	std::string nodePolicy = "widgets."+snode+".policy";
	Redis::command("hget", nodePolicy, "data_type");
	Nodes::NodeValue val;
	Parsers::NodeValueParser parser(
			Redis::toString(),
			&val,
			0x0,
			&Parsers::enum_callback <Parsers::StorageWrappingParser>);
	return (DataWrapping) val.getInt();
}

std::string getHashField(const std::string& snode)
{
	std::string nodePolicy = "widgets."+snode+".policy";
	Redis::command("hget", nodePolicy, "field");
	return Redis::toString();
}

std::string getSlotName(const std::string& snode)
{
	std::string nodePolicy = "widgets."+snode+".policy";
	Redis::command("hget", nodePolicy, "slot");
	return Redis::toString();
}

std::string dynamicLookup(std::string storage_name, std::string arg)
{
	using namespace Application;
	using namespace Enums::NodeValueTypes::RedisTypes;
	using std::string;
	string sessionid = requestSessionID(mogu()->sessionId());
	string storage_hash = Hash::toHash(storage_name);
	string node =
			session_widget(
					node_session(sessionid,storage_hash),storage_hash);

	string policy = "widgets."+storage_name+".policy";

	/* Determine whether the data in that node was supposed to have been encrypted */
	Redis::command("hget", policy, "encrypted");
	bool encrypted = Redis::getInt();

	/* Determine the type of node that we're dealing with */
	Redis::command("type", node);
	string node_type = Redis::toString();

	/* If we have a list or hash node, get the extra value that should
	 * have been passed in.
	 */
	if (node_type == REDIS_HSH)
	{
		Redis::command("hget",node,arg);
	}
	else if (node_type == REDIS_LST)
	{
		Redis::command("lindex",node,arg);
	}
	else
	{
		Redis::command("get",node);
	}
	string raw_string = Redis::toString();
	if (encrypted) return Security::decrypt(raw_string);
	return raw_string;
}

}//namespace SubmissoinHandler
}//namespace Sessions


