/*
 * ApplicationManager.cpp
 *
 *  Created on: Oct 21, 2012
 *      Author: tom
 */

#include <Types/ApplicationManager.h>
#include <TurnLeftLib/Utils/randomcharset.h>
#include <Security/Security.h>
#include <Redis/MoguQuery.h>
#include <Mogu.h>
#include <hash.h>

using namespace TurnLeft::Utils;

static const std::string INIT_SESSION = "global";

ApplicationManager::ApplicationManager(Mogu& app) : application(app)
{
	application.setSessionID(INIT_SESSION);
}

std::string ApplicationManager::newSessionID()
{
	mApp;
	RandomCharSet r;
	std::string baseID = r.generate(8);
	std::string hashedID = Hash::toHash(baseID);
	Redis::MoguQuery q(app->DBConnection());

	while (q.session_exists(hashedID))
	{
		hashedID = Hash::toHash(hashedID);
	}
	return hashedID;
}

bool ApplicationManager::verifyUser(
		const std::string& userauth, Redis::MoguQuery& q)
{
	std::string salt;
	if (!q.user_exists()) return false;
	std::string authstring_in = q.getAttemptedAuthString(userauth);
	if (!q.matchesUserAuth(authstring_in)) return false;
	std::string raw_last_auth = q.getSessionAuthToken(q.getUserLastSession());

}

bool ApplicationManager::userLogin(
		const std::string& plain_userID, const std::string& userauth)
{
	mApp;
	Redis::MoguQuery q(app->DBConnection());
	if (!verifyUser(userauth,q)) return false;
}


