/*
 * ListNodeGenerator.cpp
 *
 *  Created on: Sep 27, 2012
 *      Author: tom
 */

#include <Types/ListNodeGenerator.h>
#include <Redis/RedisCore.h>
#include <Mogu.h>
#include <Wt/WApplication>

ListNodeGenerator::ListNodeGenerator(
    const std::string& node, size_t max)
{
    __node = node;
    __current = 0;
    const char* cnode = node.c_str();
    if (!max) {
        mApp;
        app->redisCommand("llen %s", cnode);
        __max = Redis::getInt(app->reply());
    }
    else {
        __max = max;
    }

}

std::string ListNodeGenerator::next()
{
    if (__current < __max) {
        const char* cnode = __node.c_str();
        mApp;
        app->redisCommand("lindex %s %d", cnode, __current);
        ++__current;
        return Redis::toString(app->reply());
    }
    return EMPTY;
}

