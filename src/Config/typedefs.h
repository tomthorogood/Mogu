/*
 * typedefs.h
 *
 *  Created on: Sep 19, 2012
 *      Author: tom
 */

#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_
#include <unordered_map>


namespace Redis {
typedef std::vector<std::string> strvector;
}
namespace Maps {
typedef std::map<std::string, std::string> Strings;
}

typedef TurnLeft::Utils::HungryVector<char> CharVector;

typedef std::vector<std::pair<char, char> > CharCouplets;

typedef std::pair<std::string, int> TokenCycles;

typedef int (*__m_enm_parse)(
    const std::string&);

/*!\brief masked data is data whose bits are individually manipulated to
 * pack data efficiently
 */
typedef uint8_t masked;

/*!\brief mapped data is similar to masked data, however the bits themselves
 * are not masked and manipulated after initialization.
 */
typedef uint8_t mapped;

enum class Prefix {
    widgets     =1,
    data        =2,
    user        =4,
    group       =8,
    templates   =16,
    validators  =32,
    policies    =64,
    /* If adding prefixes, add them BEFORE META, and UPDATE META to reflect
     * a new bit-aligned number.
     */
    meta        =128
};

#endif /* TYPEDEFS_H_ */
