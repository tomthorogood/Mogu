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
#include <Exceptions/Exceptions.h>

namespace Redis{

using std::stringstream;
using std::string;

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

void toVector(redisReply* reply, strvector& vec)
{
    for (unsigned int i = 0; i < reply->elements; i++)
    {
        string str(reply->element[i]->str);
        vec.push_back(str);
    }
    freeReplyObject(reply);
}
} //namespace Redis


