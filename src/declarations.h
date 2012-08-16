/*
 * declarations.h
 *
 *  Created on: Jul 17, 2012
 *      Author: tom
 */

#ifndef DECLARATIONS_H_
#define DECLARATIONS_H_

#include <TurnLeftLib/TurnLeft.h>
#include <Types/Enums.h>
#include <string>
#include <exception>
#include <map>
#include <stdint.h>
#define RNS_WIDGETS 	 	"widgets"
#define RNS_SESSIONS 	 	"s"
#define RNS_EVENTS 		 	"events"
#define RNS_PERSPECTIVES 	"perspectives"
#define RNS_TEMPLATES		"templates"
#define MOGU_WIDGET(X)			RNS_WIDGETS+"."+(X)
#define MOGU_SESSION(X)			RNS_SESSIONS+"."+(X)
#define MOGU_PERSPECTIVE(X) 	RNS_PERSPECTIVES+"."+(X)
#define MOGU_TEMPLATE(X)		RNS_TEMPLATES+"."(X)
#define MOGU_GLOBAL_EVENT 		RNS_EVENTS+"."+(X)
#define MOGU_WIDGET_EVENT(X,Y) RNS_WIDGETS+"."+(X)+"."+RNS_EVENTS+"."+(Y)





/* In a single byte, this denotes the two highest order bits:
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
    class HashableAnswer_ErrorFlagParser;
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

class Mogu;
typedef TurnLeft::Utils::HungryVector <char> CharVector;
typedef std::map <std::string, Goo::Moldable*> WidgetRegister;
class redisContext;
class redisReply;

#endif /* DECLARATIONS_H_ */
