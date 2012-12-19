/*
 * MapReader.h
 *
 *  Created on: Dec 18, 2012
 *      Author: tom
 */

#ifndef MAPREADER_H_
#define MAPREADER_H_

#include <string>

template <typename E>
class SyntaxMap {
    std::map <E, std::string> __etos__;
    std::map <std::string, E> __stoe__;
public:
    SyntaxMap (E enum_entry,std::string str_entry)
    {
        __etos__[enum_entry] = str_entry;
        __stoe__[str_entry] = enum_entry;
    }

    SyntaxMap<E>& operator() (E enum_entry, std::string str_entry){
        __etos__[enum_entry] = str_entry;
        __stoe__[str_entry ] = enum_entry;
        return *this;
    }

    operator std::map <E, std::string>() {
        return __etos__;
    }

    operator std::map <std::string, E>() {
        return __stoe__;
    }

    std::string get (E enum_entry) const {
        auto iter = __etos__.find(enum_entry);
        if (iter == __etos__.end()) return "";
        else return iter->second;
    }

    E get (std::string str_entry) const {
        auto iter = __stoe__.find(str_entry);
        if (iter == __stoe__.end()) return (E) 0;
        else return iter->second;
    }
};



#endif /* MAPREADER_H_ */
