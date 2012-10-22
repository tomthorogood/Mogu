/*
 * ApplicationManager.h
 *
 *  Created on: Oct 20, 2012
 *      Author: tom
 */

#ifndef APPLICATIONMANAGER_H_
#define APPLICATIONMANAGER_H_

#include <declarations.h>

/*!\brief Manages the more important parts of each user's app instance, such
 * as the user session, session switching, and so on.
 */


class ApplicationManager
{
private:
	friend class ::Security::UserManager;
	Mogu& application;
	std::string newSessionID();
	void createNewSession(Security::AuthPackage& pkg);

public:
	ApplicationManager (Mogu& appInstance);

	/*!\brief Checks whether the supplied user info is valid and if so,
	 * logs the user in.
	 * @param userid	The plaintext user id.
	 * @param userauth	The plaintext user password.
	 * @return Whether the login was a success.
	 *
	 * This is the basic algorithm for logging a user in:
	 *
	 * + Check to ensure the user provided valid account information
	 * + Generate a new session id
	 * + Associate the new session id with the user's encrypted id
	 * + Link the new session to the previous session
	 * + Generate a new auth token and store it with the new session
	 * + Store the new auth token with the user's proofed auth string.
	 * + Change the current session to the new session.
	 */
	bool userLogin(const std::string& userid, const std::string& userauth);
};


#endif /* APPLICATIONMANAGER_H_ */
