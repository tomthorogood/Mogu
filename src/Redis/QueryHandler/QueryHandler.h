/* QueryHandler.h
 *
 * Source: QueryHandler.cpp
 *
 * Tom A. Thorogood (2013)
 */

#ifndef QUERYHANDLER_H_
#define QUERYHANDLER_H_

#include <map>
#include <string>
#include <vector>
#include <hiredis/hiredis.h>
#include "Context.h"
#include "SafeReplyObject.h"


namespace Redis {

class Query_Handler
{
public:
    Query_Handler(Context*);
    ~Query_Handler();
    void append_query(const std::string&, ...);
    void execute_query(const std::string&, ...);
    
    void flush();

    template <class T> T yield_response() {
        next_reply();
        return nullptr;
    }
    
    inline int last_response_type() const { return reply_type;}
    inline std::string reply_string() const { return reply_str_;}
    inline int reply_int() const { return reply_int_;}
    bool next_reply();
    inline void new_context(Context* context_) {
        if (c) redisFree(c);
        c = nullptr;
        context = context_;
        c = spawn_context();
    }

private:
    redisContext* spawn_context();
    void handle_reply(Safe_Reply_Object);
    void unpack_array(Safe_Reply_Object&);
    void insert_vector_element(redisReply*,std::vector<std::string>&);
    int consume_reply(redisContext*,Safe_Reply_Object&);
    
    Context* context {};
    redisContext* c {};
    int reply_type {-1};
    std::string reply_str_ {};
    int reply_int_ {};
    std::vector <std::string> reply_vec {};
    int remaining_replies {};

    // Whether the last reply was from the queue or from an immediate command
    bool queue_reply {true}; 

};

template <>
int Query_Handler::yield_response <int>();

template <>
std::string Query_Handler::yield_response <std::string>();

template <>
bool Query_Handler::yield_response <bool>();

template <>
std::vector <std::string> Query_Handler::yield_response <std::vector <std::string>>();

template <>
std::map<std::string,std::string> Query_Handler::yield_response
    <std::map <std::string,std::string>>();

}//namespace Redis

#endif //QUERYHANDLER_H_
