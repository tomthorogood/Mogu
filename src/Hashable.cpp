/*
 * Hashable.cpp
 *
 *  Created on: Jun 14, 2012
 *      Author: tom
 */

#include <Hashable.h>
#include <hash.h>

using std::string;
using Hash::toHash;

UnhashableValueException::UnhashableValueException(const string& err)
{
    string msg = "Attempted to hash a value without proper parameters set! ";
    msg.append(err);
    errMsg = msg.c_str();
}

inline const char* UnhashableValueException::what() const throw()
{
    return errMsg;
}

Hashable::Hashable(){};

Hashable::Hashable(char values[], size_t size)
{
    __is_hashable = false;
    set_values(values, size);
}

Hashable::~Hashable(){};

inline bool Hashable::is_hashable() const
{
    return __is_hashable;
}

inline void Hashable::set_values(char values[], size_t size)
{
    for (unsigned int i = 0; i < size; i++)
    {
        legal_values.add(values[i]);
    }
}

inline string Hashable::hash(const string& str)
{
    if (is_hashable())
    {
        return toHash(str);
    }
    string bad = str;
    throw UnhashableValueException(bad);
    return ""; //should never be reached...
}

void Hashable::set_flag(const char& _flag, bool on)
{
    if (on) properties.set(_flag);
    else properties.unset(_flag);
    char val = properties.value();
    for (unsigned int i = 0; i < legal_values.size(); i++)
    {
        if (val == legal_values[i])
        {
            __is_hashable=true;
        }
        else
        {
            __is_hashable=false;
        }
    }
}
