/*
 * typedefs.h
 *
 *  Created on: Sep 19, 2012
 *      Author: tom
 */

#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

namespace Bayes {
class Node;
typedef void (*NodeManipulator)(
    Node*, Node*);
}

namespace Redis {
typedef std::vector<std::string> strvector;
}
namespace Maps {
typedef std::map<std::string, std::string> Strings;
}

namespace Events {
typedef std::vector<Listener*> Listeners;
}

typedef TurnLeft::Utils::HungryVector<char> CharVector;
typedef std::map<std::string, Goo::Moldable*> WidgetRegister;

typedef std::vector<std::pair<char, char> > CharCouplets;

typedef std::pair<std::string, Enums::NodeValueTypes::NodeValueTypes> decl_pair;
typedef std::vector<decl_pair> MoguDeclaration;

typedef std::pair<std::string, int> TokenCycles;

typedef Enums::NodeValueTypes::TokenTestResult (*__m_token_test)(
    Parsers::TokenTestPackage&);

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
