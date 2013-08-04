#ifndef QUERYHANDLER_H_
#define QUERYHANDLER_H_

#include <map>
#include <string>
#include <vector>
#include <hiredis/hiredis.h>
#include "Context.h"
#include "SafeReplyObject.h"


namespace Redis {

class QueryHandler
{
public:
    QueryHandler(Context* c);
    ~QueryHandler();
    void appendQuery(const std::string&, ...);
    void executeQuery(const std::string&, ...);
    
    void flush();

    template <class T> T yieldResponse() {
        nextReply();
        return nullptr;
    }
    
    inline int lastResponseType() const { return reply_type;}
    inline std::string replyString() const { return reply_str;}
    inline int replyInt() const { return reply_int;}
    bool nextReply();
    inline void newContext(Context* context_) {
        if (c) redisFree(c);
        context = context_;
        spawnContext();
    }

private:
    redisContext* spawnContext();
    void handleReply(SafeReplyObject);
    void unpackArray(SafeReplyObject&);
    void insertVectorElement(redisReply*,std::vector<std::string>&);
    int consumeReply(redisContext*,SafeReplyObject&);
    
    Context* context;
    
    redisContext* c = nullptr;

    int reply_type          = -1;
    std::string reply_str   = "";
    int reply_int           = 0;
    std::vector <std::string> reply_vec;

    // Whether the last reply was from the queue or from an immediate command
    bool queue_reply    = true;
    int remaining_replies =0;

};

template <>
int QueryHandler::yieldResponse <int>();

template <>
std::string QueryHandler::yieldResponse <std::string>();

template <>
bool QueryHandler::yieldResponse <bool>();

template <>
std::vector <std::string> QueryHandler::yieldResponse <std::vector <std::string>>();

template <>
std::map<std::string,std::string> QueryHandler::yieldResponse
    <std::map <std::string,std::string>>();

}//namespace Redis

#endif //QUERYHANDLER_H_
