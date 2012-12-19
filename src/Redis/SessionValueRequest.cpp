/*
 * SessionValueRequest.cpp
 *
 *  Created on: Oct 28, 2012
 *      Author: tom
 */

#include <declarations.h>
#include <Mogu.h>
#include <Redis/RedisCore.h>
#include <hash.h>
#include <Redis/StoragePolicyLookup.h>
#include <Security/Encryption.h>
#include <crypt/PacketCenter.h>

#include <Redis/SessionValueRequest.h>

namespace Redis {


SessionValueRequest::SessionValueRequest(
    std::vector<std::string>& session_ids, const std::string& key,
    std::string arg)
    : lookup(key)
{
    process_request(session_ids, key, arg);
}

SessionValueRequest::SessionValueRequest(
    const std::string& key, std::string arg, std::string endpoint)
    : lookup(key)
{
    mApp;
    keyhash = Hash::toHash(key);
    node_name = EMPTY;
    retrieved_value = EMPTY;

    size_t max_iterations;
    app->redisCommand("dbsize");
    max_iterations = (size_t) redisReply_INT;

    std::string session_iter =
        (endpoint == EMPTY) ? app->sessionID() : endpoint;
    size_t num_iter = 0;

    std::vector<std::string> session_ids;
    session_ids.push_back(session_iter);
    while (session_iter != "global" && num_iter < max_iterations) {
        ++max_iterations;
        std::string tmp_node = build_session_node(session_iter, __META_HASH);
        const char* csession = tmp_node.c_str();
        app->redisCommand("hget %s %s", csession, __PREV_HASH);
        if (app->reply()->type == REDIS_REPLY_STRING) {
            session_iter = Redis::toString(app->reply());
        }
        else
            session_iter = "global";

        session_ids.push_back(session_iter);
    }

    process_request(session_ids, keyhash, arg);
}

void SessionValueRequest::process_request(
    std::vector<std::string>& session_ids, const std::string& key,
    const std::string& arg)
{
    mApp;
    size_t max_iterations = session_ids.size();
    std::string nodename;
    for (size_t i = 0; i < max_iterations; ++i) {
        std::string session_iter = session_ids[i];
        nodename = build_session_node(session_iter, key);
        const char* cnode = nodename.c_str();
        app->redisCommand("exists %s", cnode);
        if (redisReply_TRUE) {
            if (arg != EMPTY) {
                const char* carg = arg.c_str();
                if (lookup.getStorageType() == Tokens::hash) {
                    app->redisExec(Mogu::Keep, "hexists %s %s", cnode, carg);
                    if (redisReply_TRUE) break;
                }
                else if (lookup.getStorageType() == Tokens::list) {
                    app->redisExec(Mogu::Keep, "llen %s", cnode);
                    if (redisReply_INT> atoi(carg) )
                    break;
                }
                else if (lookup.getStorageType() == Tokens::set)
                {
                    app->redisCommand("sismember %s", cnode);
                    if (redisReply_TRUE) break;
                }
            }
            else if (lookup.getStorageType() != Tokens::string)
            {
                //TODO THROW ERROR
            }
            else break;
        }
    }

    node_name = nodename;
    const char* cnode = node_name.c_str();
    const char* carg = arg.c_str();

    switch (lookup.getStorageType()) {
    case Tokens::string:
        app->redisExec(Mogu::Keep, "get %s", cnode);
        break;
    case Tokens::hash:
        app->redisExec(Mogu::Keep, "hget %s %s", cnode, carg);
        break;
    case Tokens::list:
        app->redisExec(Mogu::Keep, "lindex %s %d", cnode, atoi(carg));
        break;
    default:
        break;
    }
    retrieved_value = redisReply_STRING;
    if (lookup.isEncrypted())
        retrieved_value = Security::decrypt(retrieved_value, NO_TRANSLATION);

}

}    //namespace Redis
