#include <hash.h>
#include <city.h>
#include <sstream>

namespace Hash {

HashedString::HashedString()
{
}
HashedString::~HashedString()
{
}

HashedString64::HashedString64()
    : HashedString()
{
    hashedValue = 0;
}
HashedString64::~HashedString64()
{
}

inline HashedString64& HashedString64::set(
    uint64 _value)
{
    hashedValue = _value;
    return *this;
}

inline std::string HashedString64::as_string()
{
    std::stringstream stream;
    stream << hashedValue;
    return stream.str();
}

HashedString128::HashedString128()
    : HashedString()
{
}

HashedString128::~HashedString128()
{
}
;

HashedString128& HashedString128::set( uint128 _value)
{
    hashedValue = _value;
    return *this;
}

std::string HashedString128::as_string()
{
    std::stringstream stream;
    stream << hashedValue.first << hashedValue.second;
    return stream.str();
}

HashedString* hashedString( const std::string& str)
{
    size_t len = (size_t) str.length();
    if (len > MAX_64_HASH_LENGTH) {
        HashedString128* hash = new HashedString128();
        hash->set(CityHash128(str.c_str(), len));
        return hash;
    }

    HashedString64* hash = new HashedString64();
    hash->set(CityHash64(str.c_str(), len));
    return hash;
}

std::string toHash( const std::string& str)
{
    HashedString* hash = hashedString(str);
    std::string ret = hash->as_string();
    delete hash;
    return ret;
}

}    //namespace hash
