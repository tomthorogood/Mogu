/*
 * declarations.h
 *
 *  Created on: Jul 17, 2012
 *      Author: tom
 */

#ifndef DECLARATIONS_H_
#define DECLARATIONS_H_

#define EMPTY ""

#include <TurnLeftLib/TurnLeft.h>
#include <TurnLeftLib/Utils/enumparser.h>

#include <Types/Enums.h>
#include <string>
#include <exception>
#include <utility>
#include <map>
#include <stdint.h>

#include <inline_utils.h>

/* In a single byte, this denotes the two highest order bits:
 * 	(as long as we're on a machine with the correct endianness)
 *  7 6 5 4 3 2 1 0
 *  ^ ^
 */
#define WIDGET_HO_BITS 0x3f
#define SESSION_LOOKUP "session_lookup"
#define AUTH_LOOKUP "auth_lookup"

namespace Goo
{
    class RedisInterface;
    class Moldable;
    class Dynamic;
}

namespace Exceptions
{
	class MoguException;
	class Err_UserNotFound;
	class Err_PasswordNotMatch;
}

namespace Hash
{
    class HashedString;
    class HashedString64;
    class HashedString128;
}

namespace Events
{
    class EventBindery;
    class BroadcastMessage;
    class EventNodeExtractor;
    class EventNodeProcessor;
    /* Also includes namespace ActionCenter, which contains functions only */
}

namespace Nodes
{
    class NodeValue;
}

namespace Parsers
{
    class ParseException;
    class TemplateMismatchException;
    class NodeValueParser;
    class WidgetTypeParser;
    class StyleDirectiveParser;
    class WtAlignmentParser;
    class FamilyMemberParser;
    class NodeValueTypeParser;
    class SignalActionParser;
    class SignalTriggerParser;
    class TokenGenerator;
    class MoguScript_Tokenizer;
}

namespace EventNodeConfiguration
{
    class NodeLabelParser;
}

namespace Wt
{
    class WApplication;
    class WEnvironment;
    class WAnimation;
    class WContainerWidget;
    class WStackedWidget;
    class WValidator;
    class WString;
    class WText;
    class WAnchor;
    class WLink;
    class WImage;
    class WRegExpValidator;
}

namespace Redis
{
    typedef TurnLeft::Utils::HungryVector <std::string> strvector;
}

namespace Maps
{
	typedef std::map <std::string,std::string> Strings;
}

class Mogu;
class Parseable;

typedef TurnLeft::Utils::HungryVector <char> CharVector;
typedef std::map <std::string, Goo::Moldable*> WidgetRegister;

typedef std::vector
		<std::pair <char,char> >
CharCouplets;

typedef std::vector
		<std::pair <std::string, Enums::NodeValueTypes::NodeValueTypes> >
MoguDeclaration;

/*!\brief masked data is data whose bits are individually manipulated to
 * pack data efficiently
 */
typedef uint8_t masked;

/*!\brief mapped data is similar to masked data, however the bits themselves
 * are not masked and manipulated after initialization.
 */
typedef uint8_t mapped;
class redisContext;
class redisReply;

#endif /* DECLARATIONS_H_ */
