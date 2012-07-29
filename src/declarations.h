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

namespace Goo
{
    class RedisInterface;
    class Moldable;
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
    class WString;
    class WText;
    class WAnchor;
    class WLink;
    class WImage;
}

namespace Redis
{
    typedef TurnLeft::Utils::HungryVector <std::string> strvector;
}

class Mogu;
typedef TurnLeft::Utils::HungryVector <char> CharVector;
typedef std::map <std::string, int> WidgetContents;
typedef std::map <std::string, WidgetContents> StackedWidgetTree;
class redisReply;

#endif /* DECLARATIONS_H_ */
