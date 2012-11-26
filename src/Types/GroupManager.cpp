/*
 * GroupManager.cpp
 *
 *  Created on: Nov 21, 2012
 *      Author: tom
 */

#include <Types/GroupManager.h>
#include <Redis/RedisCore.h>
#include <Static.h>


GroupManager::GroupManager(const std::string& groupid)
: __id(groupid), membershipQuery()
{
	__init__();
}

void GroupManager::__init__()
{
	mApp;
	this->app = app;
	GROUP_PREFIX = ("g."+__id) + ".";
	MEMBERSHIP = GROUP_PREFIX+"."__MEMBERSHIP_HASH;
}

std::vector <std::string>& GroupManager::getMembership()
{
	const size_t& num_levels = Application::getNumLevels();
	return __getMembership__(num_levels, 0);
}

std::vector <std::string>& GroupManager::getMembership
	(const int& max, const int& min)
{
	return __getMembership__(max, min);
}

std::vector <std::string>& GroupManager::getMembership
	(const std::string& max, const std::string& min)
{
	int maxLevel = Application::getUserLevel(max);
	int minLevel = Application::getUserLevel(min);
	return __getMembership__(maxLevel,minLevel);
}

void GroupManager::addMember(const std::string& muid, const int& rank)
{
	app->redisExec(Mogu::Discard, "zadd %s %d %s",
			MEMBERSHIP.c_str(), rank, muid.c_str());
}

void GroupManager::addMember(const std::string& muid, const std::string& rank)
{
	int rankLevel = Application::getUserLevel(rank);
	addMember(muid, rankLevel);
}

int GroupManager::getMemberRank(const std::string& muid)
{
	app->redisExec(Mogu::Keep, "zrank %s %s",
			MEMBERSHIP.c_str(), muid.c_str());
	return redisReply_INT;
}


