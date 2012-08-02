/*
 * GooRedis.h
 *
 *  Created on: Jul 14, 2012
 *      Author: tom
 */

#ifndef GOOREDIS_H_
#define GOOREDIS_H_

#ifndef REDIS_CONFIG
#define REDIS_CONFIG

#define REDIS_HOST "localhost"
#define REDIS_PORT 6379

#endif //REDIS_CONFIG


#include <declarations.h>

namespace Redis
{

void done(redisReply* reply);

std::string join (
        std::string arg1,
        std::string arg2 ="",
        std::string arg3 ="",
        std::string arg4 ="",
        std::string arg5 =""
        );

void command (
        std::string arg1,
        std::string arg2 ="",
        std::string arg3 ="",
        std::string arg4 ="",
        std::string arg5 ="");

void toVector(strvector& vec);
std::string toString();
int toInt();
long long getInt();
float toFloat();
void disconnect();

} //namespace Redis

#endif /* GOOREDIS_H_ */
