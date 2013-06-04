#ifndef QUERY_H_
#define QUERY_H_

#include <stdarg.h>
#include <declarations.h>
#include <cstring>

namespace Redis {

struct Query {
    Query() {};
    Query(std::string querystring, ...);
    Query(Query&);
    Query(std::string querystring, va_list);
    
    std::string querystring;
    const char* c_querystring = EMPTY;
    inline const char* c_str() {
        if (strcmp(c_querystring,EMPTY) ==0)
            c_querystring = querystring.c_str();
        return c_querystring;
    }

    va_list args;

    ~Query();
};

}//namespace Redis

#endif //QUERY_H_
