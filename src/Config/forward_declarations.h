/*
 * forward_declarations.h
 *
 *  Created on: Sep 19, 2012
 *      Author: tom
 */

#ifndef FORWARD_DECLARATIONS_H_
#define FORWARD_DECLARATIONS_H_

namespace Exceptions {
class MoguException;
class Err_UserNotFound;
class Err_PasswordNotMatch;
class Err_CorruptedSessionList;
}

namespace Security {
struct AuthPackage;
class LoginAuthenticator;
class UserManager;
}

namespace Redis {
struct Field;
class MoguQueryHandler;
class StorageRequeset;
class StoragePolicyLookup;
class SessionValueRequest;
class NodeEditor;
}

namespace Hash {
class HashedString;
class HashedString64;
class HashedString128;
}

namespace ActionCenter {
namespace Actions {
struct EmailPacket;
}
}

namespace Nodes {
class NodeValue;
}

namespace Parsers {
class ParseException;
class TemplateMismatchException;
class NodeValueParser;
class WtAlignmentParser;
}

namespace EventNodeConfiguration {
class NodeLabelParser;
}

namespace Wt {
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

class Mogu;
struct ContextMap;
class MoguNode;
class MoldableFactory;
class Moldable;
class Parseable;
class redisContext;
class redisReply;
class SlotManager;
class UniqueHashPackage;
class CommandValue;
class TriggerMap;
class EventHandler;
class StringMap;
class WidgetServer;
struct WidgetAssembly;

#endif /* FORWARD_DECLARATIONS_H_ */
