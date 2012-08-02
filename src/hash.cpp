#include <hash.h>
#include <city.h>
#include <sstream>

using std::string;
using std::stringstream;

namespace Hash{

HashedString::HashedString(){}
HashedString::~HashedString(){}

HashedString64::HashedString64()
:   HashedString()
{}
HashedString64::~HashedString64(){}

inline HashedString64& HashedString64::set(uint64 _value)
{
    hashedValue = _value;
    return *this;
}

inline string HashedString64::as_string()
{
    stringstream stream;
    stream << hashedValue;
    return stream.str();
}

HashedString128::HashedString128()
:   HashedString()
{}

HashedString128::~HashedString128(){};

inline HashedString128& HashedString128::set(uint128 _value)
{
    hashedValue = _value;
    return *this;
}

inline string HashedString128::as_string()
{
    stringstream stream;
    stream << hashedValue.first << hashedValue.second;
    return stream.str();
}

inline HashedString* hashedString(const string& str)
{
    size_t len = (size_t) str.length();
    if (len > MAX_64_HASH_LENGTH)
    {
        HashedString128* hash = new HashedString128();
        hash->set(
                CityHash128(str.c_str(), len)
                );
        return hash;
    }

    HashedString64* hash = new HashedString64();
    hash->set(
            CityHash64(str.c_str(), len)
            );
    return hash;
}

string toHash(const string& str)
{
    HashedString* hash = hashedString(str);
    string ret = hash->as_string();
    delete hash;
    return ret;
}

}//namespace hash
