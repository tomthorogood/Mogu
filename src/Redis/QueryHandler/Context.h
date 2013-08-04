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

#include <string>
namespace Redis {



struct Context {
    int         port;   //<!\b The port on which the redis instance is listening
    int         db_num; //<!\b The database number to use
    std::string host = ""; //<!\b The host (ip address|localhost) of the instance
    std::string auth = ""; //<!\b (optional) Database authorization
};

}//Redis

#endif /* CONTEXT_H_ */
