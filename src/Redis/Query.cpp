#include "Query.h"
#include <stdarg.h>

namespace Redis {


Query::Query(const char* querystring_, ...) : querystring(querystring_)
{
    this->querystring = querystring;
    va_start (args, querystring_);
}

Query::~Query() {
    va_end(args);
}

}//namespace Redis
