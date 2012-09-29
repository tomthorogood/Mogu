/*
 * HashNodeGenerator.cpp
 *
 *  Created on: Sep 28, 2012
 *      Author: tom
 */


#include <HashNodeGenerator.h>
#include <cassert>

HashNodeGenerator::HashNodeGenerator (const std::string& node)
{
	redisContext* rdb = redisConnect(REDIS_HOST, REDIS_PORT);
	__reply = (redisReply*) redisCommand(rdb, "type %s", node.c_str());
	assert(strcmp(__reply->str,"hash") == 0);
	freeReplyObject(__reply);

	__reply = (redisReply*) redisCommand(rdb, "hgetall %s", node.c_str());
	redisFree(rdb);
	current =0;
}
