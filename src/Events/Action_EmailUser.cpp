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

namespace Actions
{

void email_current_user(
		std::string message
		,std::string subject="Mail from FinancialFirsts.org")
{
	using namespace Sessions::SubmissionHandler;
	const std::string contact_storage = "contact";
	const std::string email_hash = Hash::toHash("contact_email");
	std::string email_address = dynamicLookup(contact_storage,email_hash);

	std::string command = "echo ";
	command += message + " |mail -s \""+subject+"\" " + email_address;
	system(command.c_str());
}


}//namespace Actions
