#include "../Actions.h"
#include "Includes.h"

namespace Actions {

void email(Moldable& broadcaster, CommandValue& v)
{
    mApp;
    NodeValue message;
    EmailManager email;
    email.setRecipient(v.get(CommandFlags::VALUE).getString());
    email.setSubject(app->slotManager().peekSlot("EMAIL_SUBJECT"));

    switch(v.getObject())
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

        case MoguSyntax::user:{
            Redis::ContextQuery db(Prefix::user);
            policyQuery(
                MoguSyntax::get
                , db
                , Prefix::user
                , v);
            message.setString(db.yieldResponse<std::string>());
            break;}

        case MoguSyntax::group:{
            Redis::ContextQuery db(Prefix::group);
            policyQuery(
                MoguSyntax::get
                , db
                , Prefix::group
                , v);
            message.setString(db.yieldResponse<std::string>());
            break;}

        case MoguSyntax::slot:{
            message.setString(app->slotManager().retrieveSlot(
                    v.getIdentifier()));
            break;}

        case MoguSyntax::data:{
            Redis::ContextQuery db(Prefix::data);
            db.appendQuery("type data.%s", v.getIdentifier().c_str());
            MoguSyntax node_type = db.yieldResponse <MoguSyntax>();
            Redis::addQuery(
                db
                , MoguSyntax::get
                , Prefix::data
                , node_type
                , v);
            message.setString(db.yieldResponse <std::string>());
            break;}

        default: return;
    }
    email.setMessage(message.getString());
    email.send();
}
}
