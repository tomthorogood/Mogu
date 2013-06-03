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
namespace Redis {



struct Context {
    int         port; //<!\b The port on which the redis instance is listening
    const char* host; //<!\b The host (ip address|localhost) of the instance
    int         db_num; //<!\b The database number to use
public:
    Context(int port_ = 6379, const char* host_ = "localhost", int db_num_=0)
    : port(port_), host(host_), db_num(db_num) {
    }
    Context(const Context& other) {
        port = other.port;
        host = other.host;
        db_num = other.db_num;
    }
};

}//Redis

#endif /* CONTEXT_H_ */
