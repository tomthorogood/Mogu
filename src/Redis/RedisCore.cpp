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
#include <cassert>

namespace Redis{

using std::stringstream;
using std::string;

namespace{
    static stringstream stream;
} //unnamed namespace

void done(redisReply* _reply, redisContext* _redis)
{
    freeReplyObject(_reply);
    redisFree(_redis);

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

void* command (
		redisContext* c,
        string arg1, string arg2, string arg3, string arg4, string arg5)
{
    std::string buff = join (arg1, arg2, arg3, arg4, arg5);
    const char* __command = buff.c_str();
    void* vr = redisCommand(c, __command);
    if (c->err)
	{
		std::cout << c->errstr << std::endl;
	}
	stream.str("");
	return vr;
}

string toString(redisReply* reply)
{
    string ret(reply->str);
    freeReplyObject(reply);
    return ret;
}

long long getInt(redisReply* reply)
{
    long long ret_int = reply->integer;
    freeReplyObject(reply);
    return ret_int;
}

float toFloat(redisReply* reply)
{
    string ret_str(reply->str);
    const char* ret_c = ret_str.c_str();
    freeReplyObject(reply);
    return atof(ret_c);
}

void toVector(redisReply* reply, strvector& vec)
{
    for (unsigned int i = 0; i < reply->elements; i++)
    {
        string str(reply->element[i]->str);
        vec.push_back(str);
    }
    freeReplyObject(reply);
}

int toInt(redisReply* reply)
{
    string ret_str(reply->str);
    const char* ret_c = ret_str.c_str();
    freeReplyObject(reply);
    return atoi(ret_c);
}
} //namespace Redis


