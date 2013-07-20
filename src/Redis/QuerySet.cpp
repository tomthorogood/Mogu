/* * QuerySet.cpp
 *
 *  Created on: Apr 3, 2013
 *      Author: tom
 */

#include "QuerySet.h"
#include "ContextMap.h"
#include <Mogu.h>
namespace Redis {

QuerySet::QuerySet(Context* context_) : context(context_)
{
    rdb = redisConnect(context->host(), context->port);
    selected_db = context->db_num;
    appendQuery("select %d", selected_db);
    execute();
}

QuerySet::QuerySet(Prefix prefix) {
    setPrefix(prefix);
}

/*!\brief Forces the return of an integer response, and continues
 * to execute redis commands until the response is not ignored.
 * If the REQUIRE_INT flag is set, and the response is a string,
 * a '0' will always be returned.
 */
template <> int
    QuerySet::yieldResponse <int> ()
{
    execute_nongreedy();

    if (reply_type == REDIS_REPLY_STRING) {
        if (last_flags & REQUIRE_INT) return 0;
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
    execute_nongreedy();
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
    execute_nongreedy();

    if (reply_type == REDIS_REPLY_STRING) {
        // Did not receive response of the proper type
        if (last_flags & REQUIRE_INT) return false;

        // Received nonempty response
        else return reply_str != EMPTY;
    }

    else if (reply_type == REDIS_REPLY_INTEGER) {
        return static_cast<bool>(reply_int);
    }

    else if (reply_type == REDIS_REPLY_ARRAY) return true;

    else return false;
}

/*!\brief Forces the return of a vector of integers,
 * and continues to execute redis commands until the response is not
 * ignored.
 */
template <> std::vector<int>
    QuerySet::yieldResponse <std::vector <int>> ()
{
    execute_nongreedy();
    return reply_array_int;
}

/*!\brief Forces the return of a vector of strings,
 * and continues to execute redis commands until the response is
 * not ignored.
 */
template <> std::vector <std::string>
    QuerySet::yieldResponse <std::vector <std::string>> ()
{
    execute_nongreedy();
    return reply_array_str;
}

void QuerySet::setPrefix(Prefix prefix)
{
    if (prefix == prefix) return;
    if (rdb != nullptr)
    {
        redisFree(rdb);
        rdb = nullptr;
    }
    mApp;
    prefix = prefix;
    context = app->contextMap()->get(prefix);
    rdb = redisConnect(context->host(), context->port);
    selected_db = context->db_num;
    clear();
    redisCommand(rdb, "select %d", selected_db);
    //appendQuery("select %d", selected_db);
    //execute();
}


}//namespace Redis

