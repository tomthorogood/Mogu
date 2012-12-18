/*
 * MapReader.h
 *
 *  Created on: Dec 18, 2012
 *      Author: tom
 */

#ifndef MAPREADER_H_
#define MAPREADER_H_

#include <unordered_map>
#include <string>

template <typename E>
class SyntaxMap {
    std::unordered_map <E, std::string> __etos__;
    std::unordered_map <std::string, E> __stoe__;

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

    operator std::unordered_map <E, std::string>() {
        return __etos__;
    }

    operator std::unordered_map <std::string, E>() {
        return __stoe__;
    }

    std::string get (E enum_entry) const {
        return __etos__[enum_entry];
    }

    E get (std::string str_entry) const {
        return __stoe__[str_entry];
    }
};



#endif /* MAPREADER_H_ */
