/*
 * ListNodeGenerator.cpp
 *
 *  Created on: Sep 27, 2012
 *      Author: tom
 */

#include <Types/ListNodeGenerator.h>
#include <Redis/RedisCore.h>

ListNodeGenerator::ListNodeGenerator(
		const std::string& node, size_t max)
{
	__node 		= node;
	__max 		= max;
	__current 	= 0;
}

std::string ListNodeGenerator::next()
{
	if (__current < __max)
	{
		Redis::command("lindex", __node, itoa(__current));
		++__current;
		return Redis::toString();
	}
	return EMPTY;
}


