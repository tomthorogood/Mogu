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
	const char* META_USERLEVELS = "meta.user_levels.list";
	static size_t num_levels =0;
}


void setUserLevels()
{
	mApp;
	app->redisExec(Mogu::Keep, "exists %s", "user_levels");
	if (!(bool)redisReply_INT) return;
	app->redisExec(Mogu::Keep, "get %s", "user_levels");

	std::string token = EMPTY;
	Parsers::TokenGenerator tgen(redisReply_STRING);
	do
	{
		token = tgen.next(',');
		TurnLeft::Utils::trimchar(token);
		app->redisExec(Mogu::Discard, "zadd %s %d %s",
				META_USERLEVELS, num_levels++, token.c_str());
	} while (token != EMPTY);
}

const size_t& getNumLevels() { return num_levels; }

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

