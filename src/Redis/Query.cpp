#include "Query.h"
#include <stdarg.h>
#include <cstring>

namespace Redis {


Query::Query(const char* querystring_, ...)
{
    strcpy(querystring,querystring_);
    this->querystring = querystring;
    va_start (args, querystring_);
}

Query::Query(Query& other)
{
    strcpy(querystring, other.querystring);
    va_copy(args,other.args);
}

Query::Query(const char* qstring, va_list args_)
{
    strcpy(querystring,qstring);
    va_copy(args, args_);
}

Query::~Query() {
    va_end(args);
}

}//namespace Redis
