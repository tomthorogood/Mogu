#ifndef QUERYSET_H_
#define QUERYSET_H_

#include <hiredis/hiredis.h>
#include <declarations.h>
#include "Query.h"
#include "Context.h"
#include <queue>
#include <memory>
namespace Redis {

class QuerySet 
{
private:
    //!\brief The flags for each query given to the set
    std::queue <uint8_t> queryflags;

    uint8_t last_flags;

    //!\brief The type of the reply returned from a Redis query
    int         reply_type = -1;

    //!\brief The type of the array returned from a relevant Redis query
    uint8_t     array_type = 0;

    //!\brief The context (populated by the Context reference passed in
    redisContext* rdb;

    //!\brief The reply from the last query sent to the Redis context
    redisReply* reply = nullptr;

    //!\brief Holds the last integer returned, where Redis stated the
    // response was an integer
    int         reply_int =-1;

    //!\brief Holds the last string returned, where Redis states the
    //response was a string.
    std::string reply_str = EMPTY;


    //!\brief Holds the last string returned where Redis stated the
    // response was a string, as a pointer to an array of chars.
    bool allow_free = false;

    //!\brief Holds a vector of the last set of responses where REdis
    // stated the response was a reply_array, and it was determined
    // that the array was made of strings
    std::vector <std::string> reply_array_str;

    //!\brief holds a vector of the last set of responses where Redis
    // stated the response was a reply_array, and it was determined
    // that the array was made of integers.
    std::vector <int> reply_array_int;


    void* vreply =nullptr;

    /*! Executes the next command sent to Redis, and
     * increases the flag iterator. 
     */
    inline redisReply* getNextReply() {
        queryflags.pop(); //reveal the next flag.
        last_flags = queryflags.front(); // make it easy to see.
        redisGetReply(rdb,&vreply);
        reply = static_cast<redisReply*>(vreply); //!< in cpp, have to explicitly cast this.
        return reply;
    }

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
    }

    /*!\brief Iterates through the multi bulk reply object, assigning
     * the elements to the proper vector. The vector is chosen based
     * upon the type of the first element. All other elements are coerced
     * into the proper type, as long as they are either an integer or string.
     * Otherwise, the elements are ignored.
     */
    inline void assignArray() {

        int num_elements = reply->elements;
        int first_element_type = reply->element[0]->type;
        for (int e = 0; e < num_elements; ++e) {
            switch (first_element_type) {
                case REDIS_REPLY_INTEGER:
                    reply_array_int.clear();
                    if (reply->element[e]->type == REDIS_REPLY_INTEGER) {
                        reply_array_int.push_back(reply->element[e]->integer);
                    }
                    else if (reply->element[e]->type == REDIS_REPLY_STRING) {
                        reply_array_int.push_back(std::atoi(reply->element[e]->str));
                    }
                    break;
                case REDIS_REPLY_STRING:
                    reply_array_str.clear();
                    if (reply->element[e]->type == REDIS_REPLY_STRING) {
                        reply_array_str.push_back(reply->element[e]->str);
                    }
                    else if (reply->element[e]->type == REDIS_REPLY_INTEGER) {
                        reply_array_str.push_back(std::to_string(reply->element[e]->integer));
                    }
                    break;
            };
        }
    }

    //!\brief Executes commands until one is executed whose reply isn't ignored
    inline void execute_nongreedy()
    {
        do {
            getNextReply();
        } while ( (last_flags & IGNORE_RESPONSE) && hasQueue() );
        assignReply();
    }

public:
    static const int IGNORE_RESPONSE        = 1;
    static const int REQUIRE_INT            = 2;
    static const int REQUIRE_STRING         = 4;

    static const uint8_t ARRAY_TYPE_INT     = 0;
    static const uint8_t ARRAY_TYPE_STR     = 1;
    
    //TODO set flag_iter to 0?
    QuerySet(Context& context);

    /*!\brief Add a query to the command queue. */
    inline void appendQuery(std::shared_ptr<Query> query,uint8_t flags=0) {
        redisvAppendCommand(rdb, query->querystring, query->args);
        queryflags.push(flags);
    }

    inline bool hasQueue() { return queryflags.size() > 1; }

    /*!\brief If no type is expected, the default is to store
     * the value of the reply object, but make no returns. 
     * Will continue to execute statements until a reply is expected.
     */
    template <class T> T yieldResponse() {
        execute_nongreedy();
        return nullptr;
    }

    inline void execute()
    {
        while (hasQueue())
        {
            getNextReply();
            assignReply();
        }
    }

    //!\brief The last reply type received.
    inline int replyType() { return reply_type;}

    //!\brief The last reply string received.
    inline std::string replyString() { return reply_str;}

    //!\brief The last reply integer received.
    inline int replyInt() { return reply_int;}

    //!\brief The last reply array of integers received.
    inline std::vector <int>& replyIntArray()
        { return reply_array_int;}
    //!\brief The last reply array of strings received.
    inline std::vector <std::string>& replyStringArray()
        { return reply_array_str;}
};

template <> int QuerySet::yieldResponse <int>();
template <> std::string QuerySet::yieldResponse <std::string>();
template <> MoguSyntax QuerySet::yieldResponse <MoguSyntax>();
template <> bool QuerySet::yieldResponse <bool>();
template <> std::vector <int> QuerySet::yieldResponse <std::vector <int>>();
template <> std::vector <std::string> QuerySet::yieldResponse <std::vector <std::string>>();

}//namespace Redis


#endif //QUERSET_H_
