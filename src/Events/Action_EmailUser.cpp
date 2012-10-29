/*
 * Action_EmailUser.cpp
 *
 *  Created on: Sep 18, 2012
 *      Author: tom
 */

#include <declarations.h>
#include <Static.h>
#include <Redis/RedisCore.h>
#include <hash.h>
#include <Events/MoldableActionCenter.h>
#include <Sessions/Lookups.h>
#include <Redis/SessionValueRequest.h>

namespace Events
{
namespace ActionCenter
{
namespace Actions
{

int send_system_email(EmailPacket* email)
{
	std::string command = "echo ";
	command += email->message
			+ " |mail -s \""
			+email->subject+"\" "
			+email->to_address;
#ifdef DEBUG
	std::cout << command << std::endl;
#endif
	return system(command.c_str());
}

std::string get_user_email(std::string username)
{
	//const std::string contact_storage = Hash::toHash("contact");
	const std::string email_hash = Hash::toHash("contact_email");
	Redis::SessionValueRequest lookup("contact", email_hash);
	return lookup.getValue();
}

int email_current_user(EmailPacket* email)
{
	std::string email_lookup_cmd = "[contact] email";
	NodeValue v;
	mApp; app->interpreter().giveInput(email_lookup_cmd, v);
	email->to_address = v.getString();
	return send_system_email(email);
}

}//namespace Actions
}//namespace ActionCenter
}//namespace Events
