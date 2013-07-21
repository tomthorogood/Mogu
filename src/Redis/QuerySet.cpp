/* * QuerySet.cpp
 *
 *  Created on: Apr 3, 2013
 *      Author: tom
 */

#include "QuerySet.h"
#include "ContextMap.h"
#include <Mogu.h>
namespace Redis {

QuerySet::QuerySet(Prefix prefix) {
    setPrefix(prefix);
}

redisReply* QuerySet::getNextReply() {
    /* Reset everything */
    --queries_queued;
    reply_array_int.clear();
    reply_array_str.clear();
    reply_str = EMPTY;
    reply_int = 0;

    redisGetReply(rdb,&vreply);
    reply = static_cast<redisReply*>(vreply);
    reply_type = reply->type;
    return reply;
}

void QuerySet::assignArray() {
    bool cleared = false; // only clear once per new array
    int num_elements = reply->elements;
    if (num_elements ==0) return;

    int first_element_type = reply->element[0]->type;

    for (int e = 0; e < num_elements; ++e) {
        switch (first_element_type) {
            case REDIS_REPLY_INTEGER:
                if (!cleared) {
                    reply_array_int.clear();
                    cleared = true;
                }
                if (reply->element[e]->type == REDIS_REPLY_INTEGER) {
                    reply_array_int.push_back(reply->element[e]->integer);
                }
                else if (reply->element[e]->type == REDIS_REPLY_STRING) {
                    reply_array_int.push_back(std::atoi(reply->element[e]->str));
                }
                break;
            case REDIS_REPLY_STRING:
                if (!cleared) {
                    reply_array_str.clear();
                    cleared = true;
                }
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

/*!\brief Forces the return of an integer response, and continues
 * to execute redis commands until the response is not ignored.
 * If the REQUIRE_INT flag is set, and the response is a string,
 * a '0' will always be returned.
 */
template <> int
    QuerySet::yieldResponse <int> ()
{
    getNextReply();
    assignReply();

    if (reply_type == REDIS_REPLY_STRING) {
        const char* cstr = reply_str.c_str();
        return std::atoi(cstr);
    }
    else if (reply_type != REDIS_REPLY_INTEGER) {
        return 0;
    }
    return reply_int;
}


/*!\brief Forces the return of a string response, and
 * continues to execute redis commands until the response is
 * not ignored. If the REQUIRE_STR flag is set, and the response
 * is an integer, the empty string will be returned.
 */
template <> std::string
    QuerySet::yieldResponse <std::string> ()
{
    getNextReply();
    assignReply();
    if (reply_type == REDIS_REPLY_INTEGER)
    {
        return std::to_string(reply_int);
    }
    return reply_str;
}

/*!\brief Forces the return of a boolean response,
 * and continues to execute redis commands until the response
 * is not ignored. If the REQUIRE_INT flag is set, and the responxse
 * is a string, false will be returned.
 */
template <> bool
    QuerySet::yieldResponse <bool> ()
{
    getNextReply();
    assignReply();

    if (reply_type == REDIS_REPLY_STRING) {
        // Received nonempty response
        return !reply_str.empty();
    }

    else if (reply_type == REDIS_REPLY_INTEGER) {
        return static_cast<bool>(reply_int);
    }

    else if (reply_type == REDIS_REPLY_ARRAY)
    {
        return (reply_array_int.size() > 1)
            || (reply_array_str.size() > 1);
    }

    else return false;
}

/*!\brief Forces the return of a vector of integers,
 * and continues to execute redis commands until the response is not
 * ignored.
 */
template <> std::vector<int>
    QuerySet::yieldResponse <std::vector <int>> ()
{
    getNextReply();
    assignReply();
    return reply_array_int;
}

/*!\brief Forces the return of a vector of strings,
 * and continues to execute redis commands until the response is
 * not ignored.
 */
template <> std::vector <std::string>
    QuerySet::yieldResponse <std::vector <std::string>> ()
{
    getNextReply();
    assignReply();
    return reply_array_str;
}

void QuerySet::setPrefix(Prefix prefix_)
{
    if (prefix == prefix_) return;
    if (rdb != nullptr)
    {
        redisFree(rdb);
        rdb = nullptr;
    }
    mApp;
    prefix = prefix_;
    context = app->contextMap()->get(prefix);
    rdb = redisConnect(context->host(), context->port);
    selected_db = context->db_num;
    clear();
    redisCommand(rdb, "select %d", selected_db);
}


}//namespace Redis

