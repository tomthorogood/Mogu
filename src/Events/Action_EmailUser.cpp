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
#include <Sessions/Submission.h>
#include <Events/MoldableActionCenter.h>
#include <Sessions/Lookups.h>

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
	return system(command.c_str());
}

std::string get_user_email(std::string username)
{
	using namespace Sessions::SubmissionHandler;
	const std::string contact_storage = Hash::toHash("contact");
	const std::string email_hash = Hash::toHash("contact_email");
	std::string usr_session = Sessions::Lookups::last_session(username);
	return userNodeLookup(usr_session, contact_storage, email_hash);
}

int email_current_user(EmailPacket* email)
{
	using namespace Sessions::SubmissionHandler;
	const std::string contact_storage = "contact";
	const std::string email_hash = Hash::toHash("contact_email");
	email->to_address = dynamicLookup(contact_storage,email_hash);
	return send_system_email(email);
}

}//namespace Actions
}//namespace ActionCenter
}//namespace Events
