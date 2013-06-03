/* * QuerySet.cpp
 *
 *  Created on: Apr 3, 2013
 *      Author: tom
 */

#include "QuerySet.h"

namespace Redis {

QuerySet::QuerySet(Context& context)
{
    rdb = redisConnect(context.host,context.port);
    auto select = std::make_shared<Query>(
        Query("select %d", context.db_num));

    // Delays the execution of this query until needed.
    appendQuery(select, IGNORE_RESPONSE);
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
        return std::atoi(reply_str.c_str());
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

template <> MoguSyntax
    QuerySet::yieldResponse <MoguSyntax>()
{
    execute_nongreedy();
    if (reply_type == REDIS_REPLY_STRING) 
        return (MoguSyntax) atoi(reply_str.c_str());
    else if (reply_type == REDIS_REPLY_INTEGER)
        return (MoguSyntax) reply_int;
    return MoguSyntax::__NONE__;
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



}//namespace Redis

