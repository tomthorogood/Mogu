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

namespace Sessions{
namespace SubmissionHandler{
using Goo::Dynamic;
using namespace Enums::SubmissionPolicies;

void absorb(Dynamic* inputWidget)
{
	namespace Widget = Enums::WidgetTypes;
	/* We will only continue if all authorization checks are passed.
	 * First, we need to determine that the Wt Application ID is the same
	 * that is being held within the private static namespace. If so,
	 * the input widget will reveal its auth token, which will be presented
	 * back to the Mogu private static namespace in order to obtain
	 * the session ID.
	 */
	std::string session_id =
			Application::requestSessionID(
			inputWidget->requestAuthorization(
					Application::handshake(
							Application::mogu()->sessionId())));

	/* Make sure that the returned session ID is not an error message. */
	if (
			(session_id.length() > 3) &&
			(session_id.substr(0,3) != "ERR"))
	{
		/* Determine the location of the node's template */
		std::string storage_locker =
				Hash::toHash(inputWidget->storageLocker());

		/* The storage node will be a hashed version of the node template name,
		 * within the current session namespace.
		 */
		std::string storageNode =
				"s."+session_id+"."+storage_locker;

		/* If widget info is stored at widgets.firstName,
		 * and the user session id is 235r308gg, and the
		 * hashed name of "firstName" is 123hu, the storage node would be:
		 * s.235r308gg.123hu
		 */
		std::string value_unenc;
		std::string value_enc;

		Widget::WidgetTypes iType = (Widget::WidgetTypes)
				inputWidget->getType();

		switch(iType & 0x3F /*Mask HO bits */){

		case Widget::input_text:{
			Wt::WLineEdit* line = (Wt::WLineEdit*) inputWidget->widget(0);
			value_unenc = line->text().toUTF8();
			break;}

		default:break;
		}

		BlowfishKeyCreator keygen;
		BF_KEY* enc_key = keygen.getKey();
		bool encrypted = requiresEncryption(inputWidget);
		if (encrypted)
		{
			PacketCenter encryption(value_unenc,DECRYPTED);
			encryption.giveKey(enc_key);
			value_enc = encryption.encrypt();
		}

		StorageMode mode_ = getStorageMode(inputWidget);
		StorageType type_ = getStorageType(inputWidget);
		DataWrapping wrap_ = getDataWrapping(inputWidget);
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
			std::string field = getHashField(inputWidget);
			if (mode_ == append)
			{
				Redis::command("hget", storageNode,field);
				std::string current_val = Redis::toString();
				if (encrypted)
				{
					PacketCenter decryption(current_val, ENCRYPTED);
					decryption.giveKey(enc_key);
					current_val = decryption.decrypt();
					std::string concat_val = current_val + " " + value_unenc;
					PacketCenter encryption(concat_val, DECRYPTED);
					encryption.giveKey(enc_key);
					value_enc = encryption.encrypt();
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
					PacketCenter decryption(current_val, ENCRYPTED);
					decryption.giveKey(enc_key);
					std::string concat_val = current_val + " " + value_unenc;
					PacketCenter encryption(concat_val, DECRYPTED);
					encryption.giveKey(enc_key);
					value_enc = encryption.encrypt();
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
			redis_command = redis_command + " " + getHashField(inputWidget);
		}
		//"command node" | "command node field" ->
		//				"command node "value"" | "command node field "value" "
		Redis::command(redis_command, value);
        Redis::clear();
	}

	else
	{
		//TODO Throw error!
	}
}

bool requiresEncryption(Dynamic* inputWidget)
{
	std::string nodePolicy =
			inputWidget->getNodeList()->at(0) + ".policy";
	Redis::command("hget", nodePolicy, "encrypted");
	Nodes::NodeValue val;
	Parsers::NodeValueParser parser(Redis::toString(), &val, inputWidget);

	return (bool) parser.getValue()->getInt();
}

StorageMode getStorageMode(Dynamic* inputWidget)
{
	std::string nodePolicy =
			inputWidget->getNodeList()->at(0)+".policy";
	Redis::command("hget", nodePolicy,"mode");
	Nodes::NodeValue val;
	Parsers::NodeValueParser parser(
			Redis::toString(),
			&val,
			inputWidget,
			&Parsers::enum_callback <Parsers::StorageModeParser>);
	return (StorageMode) parser.getValue()->getInt();
}

StorageType getStorageType(Dynamic* inputWidget)
{
	std::string nodePolicy =
			inputWidget->getNodeList()->at(0)+".policy";
	Redis::command("hget", nodePolicy, "storage_type");
	Nodes::NodeValue val;
	Parsers::NodeValueParser parser(
			Redis::toString(),
			&val,
			inputWidget,
			&Parsers::enum_callback <Parsers::StorageTypeParser>);
	return (StorageType) parser.getValue()->getInt();
}

DataWrapping getDataWrapping(Dynamic* inputWidget)
{
	std::string nodePolicy =
			inputWidget->getNodeList()->at(0)+".policy";
	Redis::command("hget", nodePolicy, "data_type");
	Nodes::NodeValue val;
	Parsers::NodeValueParser parser(
			Redis::toString(),
			&val,
			inputWidget,
			&Parsers::enum_callback <Parsers::StorageTypeParser>);
	return (DataWrapping) parser.getValue()->getInt();
}

std::string getHashField(Dynamic* inputWidget)
{
	std::string nodePolicy =
			inputWidget->getNodeList()->at(0)+".policy";
	Redis::command("hget", nodePolicy, "field");
	return Redis::toString();
}

std::string getSlotName(Dynamic* inputWidget)
{
	std::string nodePolicy =
			inputWidget->getNodeList()->at(0)+".policy";
	Redis::command("hget", nodePolicy, "slot");
	return Redis::toString();
}

}//namespace SubmissoinHandler
}//namespace Sessions


