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
bool metaKeyConfigured(std::string key)
{
	mApp;
	std::string k = "meta."+key;
	const char* ck = k.c_str();
	app->redisCommand("exists %s", ck);
	return (bool) Redis::getInt(app->reply());
}

std::string getMetaValue(std::string key)
{
	mApp;
	std::string k = "meta."+key;
	const char* ck = k.c_str();
	app->redisCommand("get %s",ck);
	return Redis::toString(app->reply());
}
}//namespace Application

