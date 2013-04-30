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
    appendQuery(select, IGNORE_RESPONSE);
}

}//namespace Redis


