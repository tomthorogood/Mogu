/*
 * HashNodeGenerator.h
 *
 *  Created on: Sep 28, 2012
 *      Author: tom
 */

#ifndef HASHNODEGENERATOR_H_
#define HASHNODEGENERATOR_H_

#include <Redis/RedisCore.h>
#include <hiredis/hiredis.h>

class HashNodeGenerator
{
	redisReply* __reply;
	size_t current;
	std::pair <std::string,std::string> _pr;
public:
	HashNodeGenerator (const std::string& node);
	inline std::pair <std::string,std::string>* next()
	{
		_pr = (current >= __reply->elements) ?
				std::make_pair(
						(std::string) EMPTY, (std::string) EMPTY)

				: std::make_pair(
						(std::string) __reply->element[current]->str,
						(std::string) __reply->element[++current]->str
				);
		++current;
		return &_pr;
	}
};


#endif /* HASHNODEGENERATOR_H_ */
