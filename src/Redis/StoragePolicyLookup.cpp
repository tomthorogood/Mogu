/*
 * StoragePolicyLookup.cpp
 *
 *  Created on: Oct 28, 2012
 *      Author: tom
 */

#include <declarations.h>
#include <Types/NodeValue.h>
#include <Redis/StoragePolicyLookup.h>
#include <Redis/RedisCore.h>
#include <Mogu.h>

namespace Redis {

std::string StoragePolicyLookup::getHashEntry(
    const std::string& node, const std::string& field)
{
    mApp;
    std::string result = EMPTY;
    app->redisExec(Mogu::Keep, "hexists %s %s", node.c_str(), field.c_str());
    if (redisReply_TRUE)
    {
        app->redisExec(Mogu::Keep, "hget %s %s", node.c_str(), field.c_str());
    }
    return result;
}

StoragePolicyLookup::StoragePolicyLookup(
    const std::string& policyName)
{
    __flags = 0;
    std::string node = buildPolicyNode(policyName);
    const char* cnode = node.c_str();
    mApp;
    NodeValue v;
    app->redisCommand("exists %s", cnode);
    if ((bool) Redis::getInt(app->reply()))
        __flags |= exists;
    else
        return;

    std::string str = getHashEntry(node, "encrypted");
    if (str != EMPTY) {
        Parsers::NodeValueParser p(str, v);
        if (v.getInt() > 0) __flags |= encrypted;
    }

    str = getHashEntry(node, "mode");
    if (str != EMPTY) {
        Parsers::NodeValueParser p(str, v);
        if (v.getInt() > 0) __flags |= set_append;
    }

    uint8_t temp = 0;

    str = getHashEntry(node, "data_type");
    if (str != EMPTY) {
        Parsers::NodeValueParser p(str, v);
        temp |= (uint8_t) v.getInt();
        temp <<= 3;
        __flags |= temp;
        temp = 0;
    }

    str = getHashEntry(node, "storage_type");
    if (str != EMPTY) {
        Parsers::NodeValueParser p(str, v, NULL);
        temp |= (uint8_t) v.getInt();
        temp <<= 6;
        __flags |= temp;
        temp = 0;
    }
}

}    //namespace Redis
