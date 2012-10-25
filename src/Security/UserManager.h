/*
 * UserCreator.h
 *
 *  Created on: Oct 22, 2012
 *      Author: tom
 */

#ifndef USERCREATOR_H_
#define USERCREATOR_H_
#include <declarations.h>

namespace Security{

class UserManager
{
	Mogu& application;
	std::string plain_userid;
	std::string enc_userid;
	std::string salt;
public:
	UserManager(Mogu& app) : application(app) {};

	/*!\brief Adds a new user for the application.
	 *
	 *
	 * @param plain_id		The user's plaintext id
	 * @param plain_passwd	The user's plaintext password
	 * @return Whether or not the creation was successful
	 *
	 * The steps for this are as follows:
	 *
	 * + Verify that the user does not already exist
	 * + Generate salt for the user
	 * + Create the user's unique auth string
	 * + Create a new session
	 * + Link the new session to the "global" session
	 * + Create a new authentication token
	 * + Link the unique auth string to the authentication token
	 * + Link the session id to the authentication token
	 *
	 */
	bool createUser(const std::string& plain_id, const std::string& plain_passwd);
};

}//namespace Security



#endif /* USERCREATOR_H_ */