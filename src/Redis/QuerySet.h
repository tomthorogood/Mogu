#ifndef QUERYSET_H_
#define QUERYSET_H_

#include <hiredis/hiredis.h>
#include <declarations.h>
#include "Context.h"
#include <queue>
#include <memory>
#include <cassert>

namespace Redis {

class QuerySet 
{
private:
    Prefix prefix           = Prefix::__NONE__;
    Context* context        = nullptr;
    bool stateful           = false;
    int queries_queued      = 0;

    //!\brief The flags for each query given to the set
    int selected_db         = 0;

    uint8_t last_flags;

    //!\brief The type of the reply returned from a Redis query
    int         reply_type  = -1;

    //!\brief The type of the array returned from a relevant Redis query
    uint8_t     array_type  = 0;

    //!\brief The context (populated by the Context reference passed in
    redisContext* rdb       = NULL;

    //!\brief The reply from the last query sent to the Redis context
    redisReply* reply       = NULL;

    //!\brief Holds the last integer returned, where Redis stated the
    // response was an integer
    int reply_int           = -1;

    //!\brief Holds the last string returned, where Redis states the
    //response was a string.
    std::string reply_str   = EMPTY;

    //!\brief Holds a vector of the last set of responses where REdis
    // stated the response was a reply_array, and it was determined
    // that the array was made of strings
    std::vector <std::string> reply_array_str;

    //!\brief holds a vector of the last set of responses where Redis
    // stated the response was a reply_array, and it was determined
    // that the array was made of integers.
    std::vector <int> reply_array_int;


    void* vreply            = NULL;

    /*! Executes the next command sent to Redis, and
     * increases the flag iterator. 
     */
    redisReply* getNextReply();

    /*!\brief Based on the type of reply received from Redis,
     * sets the correct variable to hold the state, and
     * then releases the reply object.
     */
    inline void assignReply() {
        switch(reply_type)
        {
            case REDIS_REPLY_INTEGER: 
                reply_int = reply->integer;
                break;
            case REDIS_REPLY_STATUS:
            case REDIS_REPLY_STRING:
                reply_str = reply->str;
                break;
            case REDIS_REPLY_ARRAY:
                assignArray();
                break;
            default:
                break;
        };
        freeReplyObject(reply);
        reply = NULL;
    }

    /*!\brief Iterates through the multi bulk reply object, assigning
     * the elements to the proper vector. The vector is chosen based
     * upon the type of the first element. All other elements are coerced
     * into the proper type, as long as they are either an integer or string.
     * Otherwise, the elements are ignored.
     */
     void assignArray();

public:
   
    QuerySet(){} 
    QuerySet(Prefix prefix);

    ~QuerySet() {
        if (rdb != NULL)
            redisFree(rdb);
    }

    void setPrefix(Prefix prefix);

    inline void clear() {
        reply_str = EMPTY;
        reply_int = 0;
        reply_array_str.clear();
        reply_array_int.clear();
    }

    inline void appendQuery(std::string query, ...)
    {
        ++queries_queued;
        va_list args;
        va_start(args, query);
        redisvAppendCommand(rdb, query.c_str(), args);
        va_end(args);
    }

    inline redisContext*  getContext() { return rdb; }

    /*!\brief If no type is expected, the default is to store
     * the value of the reply object, but make no returns. 
     * Will continue to execute statements until a reply is expected.
     */
    template <class T> T yieldResponse() {
        getNextReply();
        assignReply();
        return nullptr;
    }

    inline void execute()
    {
        while (queries_queued > 0)
        {
            getNextReply();
            assignReply();
        }
    }

    //!\brief The last reply type received.
    inline int replyType() { return reply_type;}

    //!\brief The last reply string received.
    inline const std::string& replyString() { return reply_str;}

    //!\brief The last reply integer received.
    inline const int& replyInt() { return reply_int;}

    //!\brief The last reply array of integers received.
    inline std::vector <int>& replyIntArray()
        { return reply_array_int;}

    //!\brief The last reply array of strings received.
    inline std::vector <std::string>& replyStringArray()
        { return reply_array_str;}
};

template <>
int QuerySet::yieldResponse <int>();

template <>
std::string QuerySet::yieldResponse <std::string>();

template <>
const SyntaxDef& QuerySet::yieldResponse <const SyntaxDef&>();

template <>
bool QuerySet::yieldResponse <bool>();

template <>
std::vector <int> QuerySet::yieldResponse <std::vector <int>>();

template <>
std::vector <std::string> QuerySet::yieldResponse <std::vector <std::string>>();

}//namespace Redis


#endif //QUERSET_H_
