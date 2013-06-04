#include "Query.h"
#include <stdarg.h>
#include <cstring>

namespace Redis {


Query::Query(std::string querystring_, ...)
: querystring(querystring_)
{
    va_start (args, querystring_);
}

Query::Query(Query& other)
: querystring(other.querystring)
{
    va_copy(args,other.args);
}

Query::Query(std::string qstring, va_list args_)
: querystring(qstring)
{
    va_copy(args, args_);
}

Query::~Query() {
    va_end(args);
}

}//namespace Redis
