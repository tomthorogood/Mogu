#ifndef QUERY_H_
#define QUERY_H_

#include <stdarg.h>

namespace Redis {

struct Query {
    Query() {};
    Query(const char* querystring, ...);
    Query(Query&);
    Query(const char* querystring, va_list);
    
    char* querystring;
    
    va_list args;

    ~Query();
};

}//namespace Redis

#endif //QUERY_H_
