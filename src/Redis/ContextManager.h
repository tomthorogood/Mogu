/*
 * ContextManager.h
 *
 *  Created on: Apr 3, 2013
 *      Author: tom
 *
 *\brief Contains the header for the Redis ContextManager, used for
 *\brief tracking Redis instances for different prefixes.
 */

#ifndef CONTEXTMANAGER_H_
#define CONTEXTMANAGER_H_

#include <declarations.h>
#include <Redis/RedisCore.h>
namespace Redis {

enum class Prefix {
    widgets,
    data,
    user,
    group,
    templates,
    validators,
    policies
};

struct Context {
    int         port; //<!\b The por on which the redis instance is listening
    const char* host; //<!\b The host (ip address|localhost) of the instance
    int         db_num; //<!\b The database number to use
public:
    Context(int port_ = 6379, const char* host_ = "localhost", int db_num_=0) {
        port = port_;
        host = host_;
        db_num = db_num_;
    }
};


class ContextManager {
public:
    ContextManager() : contextMap(){}
    inline void setPrefix(
        Prefix prefix,
        int port            = 6379,
        const char* host    = "localhost",
        int db_num          = 0)
    {
        contextMap[prefix] = std::unique_ptr <Context>(
            new Context(port,host,db_num));
    }

    inline Context& getContext(Prefix prefix)
    {
        return *contextMap.at(prefix);
    }

private:
    std::unordered_map <Prefix, std::unique_ptr<Context> > contextMap;
};

}//Redis

#endif /* CONTEXTMANAGER_H_ */
