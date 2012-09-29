/*
 * HashNodeGenerator.h
 *
 *  Created on: Sep 28, 2012
 *      Author: tom
 */

#ifndef HASHNODEGENERATOR_H_
#define HASHNODEGENERATOR_H_

#include <Redis/RedisCore.h>

class HashNodeGenerator
{
	redisReply* __reply;
	size_t current;
	std::pair <std::string,std::string> _pr;
public:
	HashNodeGenerator (const std::string& node);
	inline std::pair <std::string,std::string>* next()
	{
		if (current >= __reply->elements) return std::make_pair(EMPTY,EMPTY);
		_pr = std::make_pair(
				__reply->element[current],__reply->element[++current]);
		++current;
		return &_pr;
	}
};


#endif /* HASHNODEGENERATOR_H_ */
