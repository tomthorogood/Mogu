/*
 * ListNodeGenerator.cpp
 *
 *  Created on: Sep 27, 2012
 *      Author: tom
 */

#include <Types/ListNodeGenerator.h>
#include <Redis/RedisCore.h>
#include <Mogu.h>
#include <Wt/WApplication>

ListNodeGenerator::ListNodeGenerator(
		const std::string& node, size_t max)
{
	__node 		= node;
	__current 	= 0;

	if (!max)
	{
		mApp;
		app->redisCommand("llen", node);
		__max = Redis::getInt(app->reply());
	}
	else
	{
		__max = max;
	}


}

std::string ListNodeGenerator::next()
{
	if (__current < __max)
	{
		mApp;
		app->redisCommand("lindex", __node, itoa(__current));
		++__current;
		return Redis::toString(app->reply());
	}
	return EMPTY;
}


