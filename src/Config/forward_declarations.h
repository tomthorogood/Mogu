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
    struct MoldableTemplate;
    struct GooVariables;
}

namespace Exceptions
{
	class MoguException;
	class Err_UserNotFound;
	class Err_PasswordNotMatch;
	class Err_CorruptedSessionList;
}

namespace Security
{
	struct AuthPackage;
	class LoginAuthenticator;
	class UserManager;
}

namespace Redis
{
	class RedisStorageRequest;
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
    class EventPreprocessor;

    namespace ActionCenter
    {
    	namespace Actions
    	{
    		struct EmailPacket;
    	}
    }
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
class UniqueHashPackage;
class ApplicationManager;



#endif /* FORWARD_DECLARATIONS_H_ */
