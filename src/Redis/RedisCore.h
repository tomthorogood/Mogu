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
#include <hiredis/hiredis.h>

namespace Redis
{

void done(redisReply* reply, redisContext* redis);

std::string join (
        std::string arg1,
        std::string arg2 ="",
        std::string arg3 ="",
        std::string arg4 ="",
        std::string arg5 =""
        );

void* command (
		redisContext* c,
        std::string arg1,
        std::string arg2 ="",
        std::string arg3 ="",
        std::string arg4 ="",
        std::string arg5 ="");

void toVector(redisReply* r, strvector& vec);
std::string toString(redisReply* r);
int toInt(redisReply* r);
long long getInt(redisReply* r);
float toFloat(redisReply* r);
void clear(redisReply* r);

} //namespace Redis

#endif /* GOOREDIS_H_ */
