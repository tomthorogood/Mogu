/*
 * Static.cpp
 *
 *  Created on: Jul 29, 2012
 *      Author: tom
 */

#include <Static.h>
#include <crypt/BlowfishKey.h>
#include <crypt/PacketCenter.h>

namespace Application
{
namespace {
	Mogu* app;
	std::string __sessionID;
	std::string __auth_token;
	std::string __wt_session;
	std::map <std::string, std::string> __storageSlots;
	Goo::Moldable* __ignored_broadcaster;
	Enums::SignalTriggers::SignalTrigger __last_trigger;
}
Mogu* mogu()
{
	return app;
}

std::string encrypt(std::string unencrypted)
{
	BlowfishKeyCreator* keygen = new BlowfishKeyCreator();
	BF_KEY* key = keygen->getKey();
	PacketCenter encryption(unencrypted, DECRYPTED);
	encryption.giveKey(key);
	std::string encrypted = encryption.encrypt();
	delete keygen;
	return encrypted;
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
	if (auth_token == __auth_token)
	{
		return __sessionID;
	}
	return "ERR_NOT_AUTHORIZED";
}

void setWtSession(std::string session)
{
	__wt_session = session;
}

void setAuthToken(std::string token)
{
	__auth_token = encrypt(token);
}

void setSessionID(std::string session)
{
	__sessionID = session;
}

bool handshake(std::string wtSession)
{
	return wtSession == __wt_session;
}

void slotStorage(std::string name, std::string value)
{
	__storageSlots[name] = value;
}

std::string retrieveSlot(std::string name, std::string wtsession)
{
	if (handshake(wtsession))
	{
		std::map <std::string, std::string>::iterator iter =
				__storageSlots.find(name);
		if (iter != __storageSlots.end())
		{
			std::string ret = __storageSlots[name];
			__storageSlots.erase(name);
			return ret;
		}
		else
		{
			return "ERR_VALUE_NOT_FOUND";
		}
	}
	else
	{
		return "ERR_SESSION_AUTH_FAILURE";
	}
}

bool ignoreBroadcaster(Goo::Moldable* widget, uint8_t command)
{
	switch(command)
	{
	case 0: return widget == __ignored_broadcaster; break;
	case 1: __ignored_broadcaster =widget; return true;
	default: return false;
	}
}

Enums::SignalTriggers::SignalTrigger getLastTrigger()
{
	return __last_trigger;
}

void setLastTrigger(Enums::SignalTriggers::SignalTrigger trigger)
{
	__last_trigger = trigger;
}

}


