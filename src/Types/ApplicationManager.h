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
	Mogu& application;
	std::string newSessionID();
	bool verifyUser(
		const std::string& password, Redis::MoguQuery& q);

public:
	ApplicationManager (Mogu& appInstance);
	bool userLogin(const std::string& userid, const std::string& userauth);
};


#endif /* APPLICATIONMANAGER_H_ */
