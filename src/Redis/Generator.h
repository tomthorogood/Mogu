/*
 * Generator.h
 *
 *  Created on: Sep 28, 2012
 *      Author: tom
 */

#ifndef GENERATOR_H_
#define GENERATOR_H_

#include <declarations.h>
#include <Redis/RedisCore.h>
#include <hiredis/hiredis.h>

namespace Redis {

class Generator
{
private:
    redisContext* __redis;
protected:
    redisReply* __reply;
    size_t __current;
public:
    Generator()
    {
        __redis = redisConnect(REDIS_HOST, REDIS_PORT);
        __current = 0;
        __reply = 0;
    }
    virtual ~Generator()
    {
        freeReplyObject(__reply);
        redisFree(__redis);
    }

    inline virtual std::string next()
    {
        if (__current >= __reply->elements) return EMPTY;
        std::string nxt = __reply->element[__current]->str;
        ++__current;
        return nxt;
    }

    inline redisContext* context()
    {
        return __redis;
    }
};

}    //namespace Redis

#endif /* GENERATOR_H_ */
