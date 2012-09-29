/*
 * ListReturnGenerator.h
 *
 *  Created on: Sep 28, 2012
 *      Author: tom
 */

#ifndef LISTRETURNGENERATOR_H_
#define LISTRETURNGENERATOR_H_

namespace Redis
{

class ListReturnGenerator : public Generator
{
public:
	ListReturnGenerator(const std::string& command)
	{
		__reply = redisCommand(context(),command.c_str());
	}
};

}//namespace Redis



#endif /* LISTRETURNGENERATOR_H_ */
