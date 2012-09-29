/*
 * RedisListGenerator.h
 *
 *  Created on: Sep 28, 2012
 *      Author: tom
 */

#ifndef REDISLISTGENERATOR_H_
#define REDISLISTGENERATOR_H_

#include <Redis/RedisCore.h>

class RedisListGenerator
{
	redisReply* __reply;
	size_t current;
public:
	RedisListGenerator(const std::string& node);
	inline void next()
	{
		s
	}
};


#endif /* REDISLISTGENERATOR_H_ */
