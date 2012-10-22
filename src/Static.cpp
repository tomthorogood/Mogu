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

#include <Mogu.h>

namespace Application
{
namespace {
	std::map <std::string, std::string> __storageSlots;
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

std::string retrieveSlot(std::string name, std::string wtsession)
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

bool metaKeyConfigured(std::string key)
{
	mApp;
	app->redisCommand("exists meta."+key);
	return (bool) Redis::getInt(app->reply());
}

std::string getMetaValue(std::string key)
{
	mApp;
	app->redisCommand("get meta."+key);
	return Redis::toString(app->reply());
}
}//namespace Application

