#include "Query.h"
#include <stdarg.h>

namespace Redis {


Query::Query(const char* querystring, ...) : args()
{
    this->querystring = querystring;
    va_start (args, querystring);
}

Query::~Query() {
    va_end(args);
}

}//namespace Redis
