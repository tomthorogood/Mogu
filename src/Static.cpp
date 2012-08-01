/*
 * Static.cpp
 *
 *  Created on: Jul 29, 2012
 *      Author: tom
 */

#include <Static.h>

namespace Application
{
	namespace {
		Mogu* app;
		std::string sessionID;
		std::string auth_token;
		std::string __wt_session;
	}
	Mogu* mogu()
	{
		return app;
	}

	void defineMogu(Mogu* application)
	{
		app = application;
	}

	std::string requestSessionID(std::string auth_token)
	{
		/*Both the auth token and the session ID expire when a user leaves the
		 * application. The session ID is stored in the database indefinitely,
		 * however, whereas the auth token is lost forever. The auth token is
		 * the only way to retrieve the working session.
		 */
		if (auth_token == auth_token)
		{
			return sessionID;
		}
		return "ERR_NOT_AUTHORIZED";
	}

	void setAuthToken(std::string token)
	{
		auth_token = token;
	}

	bool handshake(std::string wtSession)
	{
		return wtSession == __wt_session;
	}
}


