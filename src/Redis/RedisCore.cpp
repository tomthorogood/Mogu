/*
 * RedisCore.cpp
 *
 *  Created on: Jul 15, 2012
 *      Author: tom
 */

#include <Redis/RedisCore.h>
#include <sstream>
#include <hiredis/hiredis.h>
#include <stdlib.h>
#include <string.h>

namespace Redis{

using std::stringstream;
using std::string;

namespace{
    redisContext* redis =0;
    redisReply* reply =0;
    stringstream stream;
} //unnamed namespace

void done(redisReply* _reply, redisContext* _redis)
{
    freeReplyObject(_reply);
    redisFree(_redis);

}

void clear()
{
    done(reply, redis);
}

std::string join (
        string arg1, string arg2, string arg3, string arg4, string arg5)
{
    stream << arg1;
    if (arg2 != "")
    {
        stream << " " << arg2;
        if (arg3 != "")
        {
            stream << " " << arg3;
            if (arg4 != "")
            {
                stream << " " << arg4;
                if (arg5 != "")
                {
                    stream << " " << arg5;
                }
            }
        }
    }
    string buff_out = stream.str();
    return buff_out;
}

void command (
        string arg1, string arg2, string arg3, string arg4, string arg5)
{
    std::string buff = join (arg1, arg2, arg3, arg4, arg5);
    const char* __command = buff.c_str();
	redis = redisConnect(REDIS_HOST, REDIS_PORT);

	if ( strcmp(__command, "") != 0)
    {
    	reply = (redisReply*) redisCommand(redis, __command);
    }
    stream.str("");
}

string toString()
{
    string ret(reply->str);
    done(reply, redis);
    return ret;
}

long long getInt()
{
    long long ret_int = reply->integer;
    done(reply, redis);
    return ret_int;
}

float toFloat()
{
    string ret_str(reply->str);
    const char* ret_c = ret_str.c_str();
    done(reply, redis);
    return atof(ret_c);
}

void toVector(strvector& vec)
{
    for (unsigned int i = 0; i < reply->elements; i++)
    {
        string str(reply->element[i]->str);
        vec.push_back(str);
    }
    done(reply, redis);
}

int toInt()
{
    string ret_str(reply->str);
    const char* ret_c = ret_str.c_str();
    done(reply, redis);
    return atoi(ret_c);
}
} //namespace Redis


