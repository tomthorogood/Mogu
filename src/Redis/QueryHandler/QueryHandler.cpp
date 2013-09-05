#include "QueryHandler.h"
#include <cassert>
#include <iostream>
namespace Redis {

Query_Handler::Query_Handler(Context* c_)
    : context (c_), c(spawn_context())
{
}

Query_Handler::~Query_Handler()
{
    if(c) redisFree(c);
}

redisContext* Query_Handler::spawn_context()
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

void Query_Handler::execute_query(const std::string& str, ...)
{
    queue_reply = false;
    redisContext* rdb {spawn_context()};
    va_list args;
    va_start(args,str);
    Safe_Reply_Object r(redisvCommand(rdb,str.c_str(),args));
    handle_reply(r);
    va_end(args);
    redisFree(rdb);
}

void Query_Handler::append_query(const std::string& str, ...)
{
    va_list args;
    va_start(args,str);
    int status {redisvAppendCommand(c,str.c_str(),args)};
    if (status != REDIS_ERR)
        ++remaining_replies;
    va_end(args);
}

void Query_Handler::handle_reply(Safe_Reply_Object r)
{
    reply_str_ = "";
    reply_int_ = 0;
    if (reply_vec.size() > 0) reply_vec.clear();
    reply_type = r.type();

    switch(reply_type)
    {
        case REDIS_REPLY_STRING:
        case REDIS_REPLY_ERROR:
        case REDIS_REPLY_STATUS:
            reply_str_ = r.str();
            break;
        case REDIS_REPLY_INTEGER:
            reply_int_ = r.integer();
            break;
        case REDIS_REPLY_ARRAY:
            unpack_array(r);
            break;
        default: 
            break;
    }
}

void Query_Handler::flush()
{
    while(remaining_replies)
    {
        next_reply();
    }
}

void Query_Handler::unpack_array(Safe_Reply_Object& r)
{
    for (int i =0; i < r.elements(); ++i)
    {
        insert_vector_element(r.element(i),reply_vec);
    }
}

void Query_Handler::insert_vector_element(redisReply* r, std::vector <std::string>& v)
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

int Query_Handler::consume_reply(redisContext* rdb, Safe_Reply_Object& r)
{
    if (!remaining_replies)
        return REDIS_REPLY_ERROR;
    
    int status = redisGetReply(rdb,r);
    if (status != REDIS_REPLY_ERROR)
        reply_type = r.type();
    --remaining_replies;
    return status;
}

bool Query_Handler::next_reply()
{
    // Since singleton queries use their own redis context,
    // we've already done all the work we need in order to
    // return a reply to the user.
    if (!queue_reply) {
        queue_reply = true;
        return true;
    }
   
    Safe_Reply_Object r {};
    int status {consume_reply(c,r)};
    if (status != REDIS_REPLY_ERROR && r)
    {
        handle_reply(r);
    }

    return status != REDIS_REPLY_ERROR;
}

template<>
int Query_Handler::yield_response <int>()
{
    if (!next_reply())
        return 0;
    else if (reply_type == REDIS_REPLY_INTEGER)
        return reply_int_;
    else if (!reply_str_.empty())
    {
        if (isdigit(reply_str_[0]))
            return atoi(reply_str_.c_str());
    }
    return 0;
}

template<>
std::string Query_Handler::yield_response <std::string>()
{
    if (!next_reply())
        return "";
    else if (
            reply_type == REDIS_REPLY_STRING
            || reply_type == REDIS_REPLY_STATUS
            || reply_type == REDIS_REPLY_ERROR)
        return reply_str_;
    else if (reply_type == REDIS_REPLY_NIL)
        return "";
    else
        return std::to_string(reply_int_);
}

template<>
std::vector <std::string> Query_Handler::yield_response <std::vector<std::string>>()
{
    next_reply();
    return reply_vec;
}

template<>
bool Query_Handler::yield_response<bool>()
{
    if (!next_reply())
        return false;
    else if (reply_type==REDIS_REPLY_INTEGER)
        return reply_int_ == 0 ? false : true;
    else if (reply_type==REDIS_REPLY_STRING)
        return !reply_str_.empty();
    else if (reply_type==REDIS_REPLY_ARRAY)
        return reply_vec.size() > 0;
    else
        return false;
}

template<>
std::map <std::string,std::string> 
    Query_Handler::yield_response<std::map<std::string,std::string>>()
{
    std::map <std::string,std::string> m {};
    if (reply_vec.size() % 2 != 0) return m;
    for (size_t i = 0; i < reply_vec.size(); ++i)
    {
        std::string key = reply_vec[i];
        std::string val = reply_vec[++i];
        m[key] = val;
    }
    return m;
}

}//namespace Redis
