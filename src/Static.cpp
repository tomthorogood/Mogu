/*
 * Static.cpp
 *
 *  Created on: Jul 29, 2012
 *      Author: tom
 */

#include <Static.h>
#include <crypt/BlowfishKey.h>
#include <crypt/PacketCenter.h>
#include <Exceptions/Exceptions.h>
#include <Redis/RedisCore.h>
#include <iostream>

namespace Application
{
namespace {
	Mogu* app;
	std::string __sessionID;
	std::string __auth_token;
	std::string __wt_session;
	std::map <std::string, std::string> __storageSlots;
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

std::string requestSessionID(std::string wtsession)
{
	/*Both the auth token and the session ID expire when a user leaves the
	 * application. The session ID is stored in the database indefinitely,
	 * however, whereas the auth token is lost forever. The auth token is
	 * the only way to retrieve the working session.
	 */
	if (wtsession == __wt_session)
	{
		return __sessionID;
	}
	return "WT_SESSION_MISMATCH";
}

std::string requestAuthToken(std::string session)
{
	if (session == __sessionID) return __auth_token;
	return "USER_SESSION_MISMATCH";
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
#ifdef DEBUG
	std::cout << "CHANGING SESSION TO THE " << session << " KEYSPACE!";
#endif
	__sessionID = session;
}

void slotStorage(std::string name, std::string value)
{
	__storageSlots[name] = value;
}

std::string retrieveSlot(std::string name, std::string wtsession)
{
	if (wtsession == __wt_session)
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
			throw Exceptions::Err_SlotLookupFailed(name,
					"No information exists at that slot ID.");
		}
	}
	else
	{
		throw Exceptions::Err_WtAuthFailure(
				wtsession,
				"Wt Session ID Mismatch! This is not a good thing!");
	}
}

bool metaKeyConfigured(std::string key)
{
	Redis::command("exists meta."+key);
	return (bool) Redis::getInt();
}

std::string getMetaValue(std::string key)
{
	Redis::command("get meta."+key);
	return Redis::toString();
}
}//namespace Application

