/*
 * HashKeyGenerator.h
 *
 *  Created on: Sep 28, 2012
 *      Author: tom
 */

#ifndef HASHKEYGENERATOR_H_
#define HASHKEYGENERATOR_H_

namespace Redis {

class HashKeyGenerator: public Generator
{
    std::pair<std::string, std::string> __pr;
public:
    HashKeyGenerator(
        const std::string& keyName)
    {
        __reply = redisCommand(context(), "hgetall %s", keyName.c_str());
    }

    inline std::pair<std::string, std::string>* getPair()
    {
        __pr = std::make_pair(next(), next());
        return &__pr;
    }
};

}

#endif /* HASHKEYGENERATOR_H_ */
