#include "QueryHandler.h"
#include <cassert>
#include <iostream>
namespace Redis {

QueryHandler::QueryHandler(Context* c_)
    : context (c_), c(spawnContext())
{
}

QueryHandler::~QueryHandler()
{
    redisFree(c);
}

redisContext* QueryHandler::spawnContext()
{
    if (!context) return nullptr;
    redisContext* rdb = redisConnect(context->host.c_str(), context->port);
    freeReplyObject((redisReply*)
        redisCommand(rdb, "select %d", context->db_num));
    
    if (!context->auth.empty())
        freeReplyObject((redisReply*)
                redisCommand(rdb,"auth %s", context->auth.c_str()));

    return rdb;
}

void QueryHandler::executeQuery(const std::string& str, ...)
{
    queue_reply = false;
    redisContext* rdb = spawnContext();
    va_list args;
    va_start(args,str);
    SafeReplyObject r(redisvCommand(rdb,str.c_str(),args));
    handleReply(r);
    va_end(args);
    redisFree(rdb);
}

void QueryHandler::appendQuery(const std::string& str, ...)
{
    va_list args;
    va_start(args,str);
    int status = redisvAppendCommand(c,str.c_str(),args);
    if (status != REDIS_ERR)
        ++remaining_replies;
    va_end(args);
}

void QueryHandler::handleReply(SafeReplyObject r)
{
    reply_str = "";
    reply_int = 0;
    if (reply_vec.size() > 0) reply_vec.clear();
    reply_type = r.type();

    switch(reply_type)
    {
        case REDIS_REPLY_STRING:
        case REDIS_REPLY_ERROR:
        case REDIS_REPLY_STATUS:
            reply_str = r.str();
            break;
        case REDIS_REPLY_INTEGER:
            reply_int = r.integer();
            break;
        case REDIS_REPLY_ARRAY:
            unpackArray(r);
            break;
        default: 
            break;
    }
}

void QueryHandler::flush()
{
    while(remaining_replies)
    {
        nextReply();
    }
}

void QueryHandler::unpackArray(SafeReplyObject& r)
{
    for (int i =0; i < r.elements(); ++i)
    {
        insertVectorElement(r.element(i),reply_vec);
    }
}

void QueryHandler::insertVectorElement(redisReply* r, std::vector <std::string>& v)
{
    if (!r) return;
    switch(r->type)
    {
    case REDIS_REPLY_STRING:
    case REDIS_REPLY_ERROR:
    case REDIS_REPLY_STATUS:
        v.push_back(r->str);
        break;
    case REDIS_REPLY_INTEGER:
        v.push_back(std::to_string(r->integer)); 
        break;
    default: 
        break;
    }
}

int QueryHandler::consumeReply(redisContext* rdb, SafeReplyObject& r)
{
    if (!remaining_replies)
        return REDIS_REPLY_ERROR;
    
    int status = redisGetReply(rdb,r);
    if (status != REDIS_REPLY_ERROR)
        reply_type = r.type();
    --remaining_replies;
    return status;
}

bool QueryHandler::nextReply()
{
    // Since singleton queries use their own redis context,
    // we've already done all the work we need in order to
    // return a reply to the user.
    if (!queue_reply) {
        queue_reply = true;
        return true;
    }
   
    SafeReplyObject r; 
    int status = (consumeReply(c,r));
    if (status != REDIS_REPLY_ERROR && r)
    {
        handleReply(r);
    }

    return status != REDIS_REPLY_ERROR;
}

template<>
int QueryHandler::yieldResponse <int>()
{
    if (!nextReply())
        return 0;
    else if (reply_type == REDIS_REPLY_INTEGER)
        return reply_int;
    else if (!reply_str.empty())
    {
        if (isdigit(reply_str[0]))
            return atoi(reply_str.c_str());
    }
    return 0;
}

template<>
std::string QueryHandler::yieldResponse <std::string>()
{
    if (!nextReply())
        return "";
    else if (
            reply_type == REDIS_REPLY_STRING
            || reply_type == REDIS_REPLY_STATUS
            || reply_type == REDIS_REPLY_ERROR)
        return reply_str;
    else if (reply_type == REDIS_REPLY_NIL)
        return "";
    else
        return std::to_string(reply_int);
}

template<>
std::vector <std::string> QueryHandler::yieldResponse <std::vector<std::string>>()
{
    nextReply();
    return reply_vec;
}

template<>
bool QueryHandler::yieldResponse<bool>()
{
    if (!nextReply())
        return false;
    else if (reply_type==REDIS_REPLY_INTEGER)
        return static_cast<bool>(reply_int);
    else if (reply_type==REDIS_REPLY_STRING)
        return !reply_str.empty();
    else if (reply_type==REDIS_REPLY_ARRAY)
        return reply_vec.size() > 0;
    else
        return false;

}

template<>
std::map <std::string,std::string> 
    QueryHandler::yieldResponse<std::map<std::string,std::string>>()
{
    std::map <std::string,std::string> m;
    if (reply_vec.size() % 2 != 0) return m;
    for (int i = 0; i < reply_vec.size(); ++i)
    {
        std::string key = reply_vec[i];
        std::string val = reply_vec[++i];
        m[key] = val;
    }
    return m;
}

}//namespace Redis
