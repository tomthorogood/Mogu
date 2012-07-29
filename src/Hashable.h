/*
 * Hashable.cpp
 *
 *  Created on: Jun 14, 2012
 *      Author: tom
 */

#ifndef HASHABLE_CPP_
#define HASHABLE_CPP_

#include <declarations.h>

class UnhashableValueException : public std::exception
{
private:
    const char* errMsg;
public:
    UnhashableValueException (const std::string& err);
    const char* what() const throw();
};


class Hashable
{
private:
    CharVector legal_values;
    TurnLeft::Utils::PackedData <char> properties;
    bool __is_hashable;

public:
    Hashable();
    Hashable (char values[], size_t size);
    virtual ~Hashable();

    bool is_hashable() const;

    virtual void set_flag(const char& _flag, bool on=true);
    void set_values(char values[], size_t size);
    std::string hash (const std::string& str);
};

#endif /* HASHABLE_CPP_ */
