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

#endif /* TYPEDEFS_H_ */
