#include "Query.h"

namespace Redis {


Query::Query(const char* querstring, ...) 
{
    this->querystring = querystring;
    va_start (args, querystring);
}

Query::~Query() {
    va_end(args);
}

}//namespace Redis
