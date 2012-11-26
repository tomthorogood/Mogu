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
#include <Parsers/TokenGenerator.h>
#include <TurnLeftLib/Utils/inlines.h>
#include <iostream>

#include <Mogu.h>

namespace Application
{

namespace {
	static bool configured = false;
	const char* META_USERLEVELS = "meta.user_levels";
	static size_t num_levels =0;
}


void setUserLevels()
{
	mApp;
	app->redisExec(Mogu::Keep, "exists %s", META_USERLEVELS);
	if (!(bool)redisReply_INT) return;
	app->redisExec(Mogu::Keep, "get %s", META_USERLEVELS);
	std::vector <std::string> vec_userlevels;
	std::string token = EMPTY;
	Parsers::TokenGenerator tgen(redisReply_STRING);
	do
	{
		token = tgen.next(',');
		TurnLeft::Utils::trimchar(token);
		if (token != EMPTY) vec_userlevels.push_back(token);
	} while (token != EMPTY);

	size_t num_userlevels = vec_userlevels.size();

	// Replace the string key with the sorted set of ranked user levels
	app->redisExec(Mogu::Discard, "del %s", META_USERLEVELS);
	for (size_t level = num_userlevels-1; level >=0; --level)
	{
		/* Essentially, reverse the order of the list. This makes it easier
		 * for users to add levels and also makes testing of ranks much
		 * easier.
		 */
		app->redisExec(Mogu::Discard, "zadd %s %d %s",
			META_USERLEVELS, level, vec_userlevels[level].c_str());
	}
}

std::string getUserLevel(const int& index)
{
	mApp;
	app->redisExec(Mogu::Keep, "zrangebyscore %s %d %d",
			META_USERLEVELS, index, index);
	return app->reply()->element[0]->str;
}

int getUserLevel(const std::string& name)
{
	mApp;
	app->redisExec(Mogu::Keep, "zrank %s %s",
			META_USERLEVELS, name.c_str());
	return redisReply_INT;
}


void configure()
{
	if (configured) return;
	setUserLevels();
	configured = true;
}

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

