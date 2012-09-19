/*
 * forward_declarations.h
 *
 *  Created on: Sep 19, 2012
 *      Author: tom
 */

#ifndef FORWARD_DECLARATIONS_H_
#define FORWARD_DECLARATIONS_H_

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
    struct TokenTestPackage;
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

namespace Bayes{
	class Network;
	class Node;
	class Edge;
	class Manipulation;

}


class Mogu;
class Parseable;
class redisContext;
class redisReply;



#endif /* FORWARD_DECLARATIONS_H_ */
