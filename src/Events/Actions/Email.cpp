#include "../Actions.h"
#include "Includes.h"
#include <Redis/NodeEditor.h>
namespace Actions {

void email(Moldable& broadcaster, CommandValue& v)
{
    mApp;
    MoguSyntax object = (MoguSyntax) v.get(CommandFlags::OBJECT);
    std::string identifier = (std::string) v.get(CommandFlags::IDENTIFIER);
    NodeValue arg;
    if (v.test(CommandFlags::ARG))
        arg = v.get(CommandFlags::ARG);
    Redis::NodeEditor node(object, identifier, &arg);
    NodeValue message;
    EmailManager email;
    email.setRecipient(v.get(CommandFlags::VALUE).getString());
    email.setSubject(app->slotManager().peekSlot("EMAIL_SUBJECT"));

    switch((MoguSyntax) v.get(CommandFlags::OBJECT))
    {
        case MoguSyntax::own:{
            broadcaster.getAttribute( (MoguSyntax) v.get(CommandFlags::ARG), message);
            email.setMessage(message.getString());
            break;}

        case MoguSyntax::widget:{
            Moldable* widget = app->registeredWidget(v.getIdentifier());
            widget->getAttribute( (MoguSyntax) v.get(CommandFlags::ARG), message);
            email.setMessage(message.getString());
            break;}

        case MoguSyntax::data: 
        case MoguSyntax::group:
        case MoguSyntax::user:
            message.setString(node.read());
            break;
        case MoguSyntax::slot:{
            message.setString(app->slotManager().retrieveSlot(
                    v.getIdentifier()));
            break;}
        default: return;
    }
    email.setMessage(message.getString());
    email.send();
}
}
