/*
 * MapReader.h
 *
 *  Created on: Dec 18, 2012
 *      Author: tom
 */

#ifndef MAPREADER_H_
#define MAPREADER_H_

#include <string>
#include <unordered_map>


class SyntaxRangeException : public std::exception {
public:
    int oor;
    SyntaxRangeException(int i) { oor = i;}
    const char* what() const throw()
    {
        std::string s_oor = itoa(oor);
        std::string message = "Syntax Enumerator out of range: ";
        message += s_oor;
        return message.c_str();
    }
};

class InvalidTokenException : public std::exception {
public:
    std::string s_invalid;
    InvalidTokenException(std::string invalid) {s_invalid = invalid;}
    const char* what() const throw()
    {
        std::string message = "Invalid token encountered: " + s_invalid;
        return message.c_str();
    }
};

template <typename E>
class SyntaxMap {
    std::unordered_map <E, std::string, std::hash<int> > __etos__;
    std::unordered_map <std::string, E> __stoe__;
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

    operator std::unordered_map <E, std::string>() {
        return __etos__;
    }

    operator std::unordered_map <std::string, E>() {
        return __stoe__;
    }

    std::string get (E enum_entry) const {
        std::string result;
        try
        {
            result = __etos__.at(enum_entry);
        }
        catch (...)
        {
            throw SyntaxRangeException(enum_entry);
        }
        return result;
    }

    E get (std::string str_entry) const {
        E result;
        try
        {
            result = __stoe__.at(str_entry);
        }
        catch (...)
        {
            throw InvalidTokenException(str_entry);
        }
        return result;
    }

    bool test() {
        bool stoe = true;
        bool etos = true;

        for (auto iter = __stoe__.begin(); iter != __stoe__.end(); ++iter)
        {
            std::string strval = iter->first;
            if (strval != get(iter->second)) {
                stoe = false;
                break;
            }
        }

        for (auto iter = __etos__.begin(); iter != __etos__.end(); ++iter)
        {
            E enumval = iter->first;
            if (enumval != get(iter->second)) {
                etos = false;
                break;
            }
        }

        return (etos && stoe);
    }
};



#endif /* MAPREADER_H_ */
