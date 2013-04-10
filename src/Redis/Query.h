#include <declarations.h>
namespace Redis {

struct Query {
        Query(const char* querystring, ...);
        const char* querystring;
        va_list args;

        ~Query();
};

}//namespace Redis
