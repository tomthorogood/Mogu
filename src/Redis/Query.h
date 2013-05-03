#ifndef QUERY_H_
#define QUERY_H_

#include <stdarg.h>

namespace Redis {

struct Query {
    Query(const char* querystring, ...);
    
    const char* querystring;
    
    va_list args;

    ~Query();
};

}//namespace Redis

#endif //QUERY_H_
