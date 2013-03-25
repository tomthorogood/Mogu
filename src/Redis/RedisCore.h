/*
 * RedisCore.h
 *
 *  Created on: Jul 14, 2012
 *      Author: tom
 */

#ifndef MOGUREDIS_H_
#define MOGUREDIS_H_

#ifndef REDIS_CONFIG
#define REDIS_CONFIG

#define REDIS_HOST "localhost"
#define REDIS_PORT 6379

#endif //REDIS_CONFIG

#include <declarations.h>
#include <hiredis/hiredis.h>
#include "QuerySet.h"

namespace Redis {

void toVector(
    redisReply* r, strvector& vec);
std::string toString(
    redisReply* r);
int toInt(
    redisReply* r);
long long getInt(
    redisReply* r);
float toFloat(
    redisReply* r);
void clear(
    redisReply* r);
inline bool getBool(
    redisReply* r)
{
    return (bool) getInt(r);
}

}    //namespace Redis

#endif /* MOGUREDIS_H_ */
