/*
 * hash.h
 *
 *  Created on: Jun 11, 2012
 *      Author: tom
 */

#ifndef HASH_H_
#define HASH_H_

#include <city.h>
#include <declarations.h>

namespace Hash {

/*!\brief Recommended by the City Hash developers. */
#define MAX_64_HASH_LENGTH 2000

/*!\brief A base polymorphic class to obtain hashed values from the City Hash library. */
class HashedString
{
public:
    HashedString();
    virtual ~HashedString();

    /*!\brief Returns the string representation of the hashed value. */
    virtual std::string as_string() =0;
};

/*!\brief The 64-bit implementation of the HashedString base class,
 * deals with uint64 values.
 */
class HashedString64: public HashedString
{
private:
    uint64 hashedValue;
public:
    HashedString64();
    virtual ~HashedString64();
    HashedString64& set(
        uint64 _value);
    std::string as_string();
};

/*!\brief The 128-bit implementation of the HashedString base class,
 * which deals with uint128 values.
 */
class HashedString128: public HashedString
{
private:
    uint128 hashedValue;
public:
    HashedString128();
    virtual ~HashedString128();
    HashedString128& set(
        uint128 _value);
    std::string as_string();
};

/*!\brief Returns a pointer to a HashedString object. Note that this
 * pointer MUST be deleted in order to avoid memory leakage!
 * @param str The string to be hashed.
 * @return A pointer to a HashedString object.
 */
HashedString* hashedString(
    const std::string& str);

/*!\brief hashes a string of any length! Utilizes the City Hash lib due to its
 * high performance rating and the fact that cryptography is not an issue for
 * these purposes. If the string passed in is greater than 2000 characters, the
 * CityHash64 function will be used, per the recommendation of the City Hash
 * authors.
 * @param str The string to be hashed.
 * @return The hashed value.
 */
std::string toHash(
    const std::string& str);

}    //<-- namespace hash

#endif /* HASH_H_ */
