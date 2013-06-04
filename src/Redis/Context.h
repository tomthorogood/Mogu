/*
 * ContextManager.h
 *
 *  Created on: Apr 3, 2013
 *      Author: tom
 *
 *\brief Contains the header for the Redis ContextManager, used for
 *\brief tracking Redis instances for different prefixes.
 */

#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <declarations.h>
#include <cstring>
namespace Redis {



struct Context {
    int         port; //<!\b The port on which the redis instance is listening
    std::string host_; //<!\b The host (ip address|localhost) of the instance
    const char* c_host = EMPTY;
    int         db_num; //<!\b The database number to use
public:
    Context(int port_ = 6379, std::string host__ = "localhost", int db_num_=0)
    : port(port_), host_(host__), db_num(db_num_) {
    }
    Context(const Context& other) {
        port = other.port;
        host_ = other.host_;
        db_num = other.db_num;
    }
    inline const char* host() {
        if (strcmp(c_host,EMPTY)==0)
            c_host = host_.c_str();
        return c_host;
    }
};

}//Redis

#endif /* CONTEXT_H_ */
