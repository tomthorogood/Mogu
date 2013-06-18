/*
 * Actions.cpp
 *
 *  Created on: Apr 22, 2013
 *      Author: tom
 */

#include <Mogu.h>
#include "Actions.h"
#include "CommandValue.h"
#include <Moldable/MoldableAbstractParent.h>
#include <Redis/QueryAssistant.h>
#include <Types/SlotManager.h>
#include <Types/EmailManager.h>
namespace Actions {
MoguSyntax getPolicyNodeType(const std::string& identifier)
{
    Redis::ContextQuery policydb(Prefix::policies);
    policydb.appendQuery("hget policies.%s %d", identifier.c_str(),
            MoguSyntax::type);
    return policydb.yieldResponse<MoguSyntax>();
}

void policyQuery(
        MoguSyntax queryType
        , Redis::ContextQuery& db
        , Prefix prefix, CommandValue& v
        , bool init = true)
{
    mApp;
    std::string group = std::to_string(app->getGroup());

    MoguSyntax node_type = getPolicyNodeType(v.getIdentifier());
    if (!init) goto lbl_append_query;
    switch(prefix)
    {
        case Prefix::user:
            v.setIdentifier(
                    app->getUser()
                    + std::string(".")
                    + v.getIdentifier());
            break;
        case Prefix::group:
            v.setIdentifier( group + "." + v.getIdentifier());
            break;
        default:return;
    }
lbl_append_query:
    Redis::addQuery(
        db
        , queryType
        , prefix
        , node_type
        , v);
}

void set (Moldable& broadcaster, CommandValue& v)
{
    mApp;
    switch(v.getObject())
    {
        case MoguSyntax::own:{  // set attribute of calling widget
            MoguSyntax attr = (MoguSyntax) v.getArg();
            broadcaster.setAttribute(attr,v.getValue());
            break;}
        case MoguSyntax::path:{ // set application URL
            mApp;
            app->setPath((std::string) v.getValue());
            break;}
        case MoguSyntax::user:{ // set user field
            mApp;
            if (v.getIdentifier() == EMPTY)
            {
                app->getUserManager().loginUser(
                    app->slotManager().retrieveSlot("USERNAME"),
                    app->slotManager().retrieveSlot("PASSWORD"));
                return;
            }
            Redis::ContextQuery db(Prefix::user);
            policyQuery(
                    MoguSyntax::set
                    , db
                    , Prefix::user
                    , v);
            db.execute();
            break;}
        case MoguSyntax::group:{// set group field
            mApp;
            if (v.getIdentifier() == EMPTY)
            {
                app->getUserManager().setActiveGroup(v.getValue().getString());
                return;
            }
            Redis::ContextQuery db(Prefix::group);
            policyQuery(
                MoguSyntax::set
                , db
                , Prefix::group
                , v);
            db.execute();
            break;}
        case MoguSyntax::data:{ // set data field
            Redis::ContextQuery db(Prefix::data);
            db.appendQuery("type data.%s", v.getIdentifier().c_str());
            MoguSyntax node_type = db.yieldResponse <MoguSyntax> ();
            Redis::addQuery(
                db
                , MoguSyntax::set 
                , Prefix::data
                , node_type
                , v);
            db.execute();                    
            break;}
        case MoguSyntax::slot:{ // set application slot
            mApp;
            app->slotManager().setSlot(v.getIdentifier(), v.getValue());
            break;}
        case MoguSyntax::widget:{   // set arbitrary widget attribute
            mApp;
            Moldable* widget = app->registeredWidget(v.getIdentifier());
            if (widget == nullptr) break;
            widget->setAttribute((MoguSyntax) v.getArg(), v.getValue());
            break;}
        default: return; // If bad input, stop immediately
    }
}

void increment (Moldable& broadcaster, CommandValue& v)
{
    int value = (v.getFlags() & v.VALUE) ?
        v.getValue().getInt() : 1;
    NodeValue final;
    switch(v.getObject())
    {
        case MoguSyntax::own:{
            NodeValue current;
            broadcaster.getAttribute( (MoguSyntax) v.getArg(), current);
            if (current.getType() == ReadType::string_value)
                current.setInt(atoi(current.getString().c_str()));
            current.setInt(current.getInt()+value);
            broadcaster.setAttribute( 
                    (MoguSyntax) v.getArg(), current);
            break;}

        case MoguSyntax::user:{
            Redis::ContextQuery db(Prefix::user);
            policyQuery(
                MoguSyntax::get
                , db
                , Prefix::user
                , v);
            policyQuery(
                MoguSyntax::set
                , db
                , Prefix::user
                , v);

            final.setInt(
                final.getInt() + atoi(db.yieldResponse<std::string>().c_str())
            );
            db.execute();
            break;}

        case MoguSyntax::group:{
            Redis::ContextQuery db(Prefix::group);
            policyQuery(
                MoguSyntax::get
                , db
                , Prefix::group
                , v);
            policyQuery(
                MoguSyntax::set
                , db
                , Prefix::group
                , v);

            final.setInt(
                final.getInt() + atoi(db.yieldResponse<std::string>().c_str())
            );
            db.execute();
            break;}

        case MoguSyntax::data:{
            Redis::ContextQuery db(Prefix::data);
            final.setInt(value);
            db.appendQuery("type data.%s", v.getIdentifier().c_str());
            MoguSyntax node_type = db.yieldResponse <MoguSyntax>();
            v.setValue(final);
            Redis::addQuery(
                db
                , MoguSyntax::get
                , Prefix::data
                , node_type
                , v);
            Redis::addQuery(
                db
                , MoguSyntax::set
                , Prefix::data
                , node_type
                , v);
            final.setInt(
                final.getInt() + atoi(db.yieldResponse<std::string>().c_str())
            );
            db.execute();

            break;}
        case MoguSyntax::slot:{
            mApp;
            final.setInt(
                app->slotManager().retrieveSlot(v.getIdentifier()).getInt());
            final.setInt(final.getInt()+value);
            app->slotManager().setSlot(
                    v.getIdentifier(), final);
            break;}

        case MoguSyntax::widget:{
            mApp;
            Moldable* widget = app->registeredWidget(v.getIdentifier());
            NodeValue current;
            widget->getAttribute( (MoguSyntax) v.getArg(), current);
            if (current.getType() == ReadType::string_value)
                current.setInt(atoi(current.getString().c_str()));
            current.setInt(current.getInt()+value);
            widget->setAttribute( (MoguSyntax) v.getArg(), current );
            break;}
        default: return;
    }
} 

void decrement (Moldable& broadcaster, CommandValue& v)
{
    NodeValue final;
    int value = (v.getFlags() & v.VALUE) ?
        v.getValue().getInt() : 1;
    switch(v.getObject())
    {
        case MoguSyntax::own:{
            NodeValue current;
            broadcaster.getAttribute( (MoguSyntax) v.getArg(), current);
            if (current.getType() == ReadType::string_value)
                current.setInt(atoi(current.getString().c_str()));
            current.setInt(current.getInt()-value);
            broadcaster.setAttribute( (MoguSyntax) v.getArg(), current);
            break;}

        case MoguSyntax::user:{
            Redis::ContextQuery db(Prefix::user);
            policyQuery(
                MoguSyntax::get
                , db
                , Prefix::user
                , v);
            policyQuery(
                MoguSyntax::set
                , db
                , Prefix::user
                , v);
            final.setInt(
                final.getInt() - atoi(db.yieldResponse<std::string>().c_str())
            );
            db.execute();
            break;}

        case MoguSyntax::group:{
            Redis::ContextQuery db(Prefix::user);
            policyQuery(
                MoguSyntax::get
                , db
                , Prefix::user
                , v);
            policyQuery(
                MoguSyntax::set
                , db
                , Prefix::user
                , v);
            final.setInt(
                final.getInt() - atoi(db.yieldResponse<std::string>().c_str())
            );
            db.execute();
            break;}

        case MoguSyntax::data:{
            Redis::ContextQuery db(Prefix::data);
            final.setInt(value);
            v.setValue(final);
            db.appendQuery("type data.%s", v.getIdentifier().c_str());
            MoguSyntax node_type = db.yieldResponse<MoguSyntax>();
            Redis::addQuery(
                db
                , MoguSyntax::get
                , Prefix::data
                , node_type
                , v);
            Redis::addQuery(
                db
                , MoguSyntax::set
                , Prefix::data
                , node_type
                , v);
            final.setInt(
                final.getInt() - atoi(db.yieldResponse<std::string>().c_str())
            );
            db.execute();
            break;}

        case MoguSyntax::slot:{
            mApp;
            final.setString(app->slotManager().retrieveSlot(v.getIdentifier()));
            final.setInt(atoi(final.getString().c_str()));
            final.setInt(final.getInt()-value);
            app->slotManager().setSlot(
                    v.getIdentifier(), final);
            break;}

        case MoguSyntax::widget:{
            mApp;
            Moldable* widget = app->registeredWidget(v.getIdentifier());
            NodeValue current;
            widget->getAttribute( (MoguSyntax) v.getArg(), current);
            if (current.getType() == ReadType::string_value)
                current.setInt(atoi(current.getString().c_str()));
            current.setInt(current.getInt()-value);
            widget->setAttribute( (MoguSyntax) v.getArg(), current );
            break;}

        default: return;
    }
} 

void test(Moldable& broadcaster, CommandValue& v)
{
    mApp;

    bool result = false;
    NodeValue value;

    // This is a little bit of reverse engineering, because it undoes some of
    // tokenization that's already been done, which is inefficient, but it's
    // much easier to understand than the previous implementation.
    std::string state_str = v.stitchState();

    app->interpreter().giveInput(state_str, value);

    result = (value == v.getValue());
    if (!result) broadcaster.fail().emit();
    else broadcaster.succeed().emit();
}

void reload(Moldable& broadcaster, CommandValue& v)
{
    switch(v.getObject())
    {
        case MoguSyntax::own:{
            broadcaster.reload();
            break;}
        case MoguSyntax::widget:{
            mApp;
            Moldable* widget = app->registeredWidget(v.getIdentifier());
            widget->reload();
            break;}
        default: return;
    }
}

void append(Moldable& broadcaster, CommandValue& v)
{
    if (v.getValue().getType() == ReadType::int_value)
        v.getValue().setString(std::to_string(v.getValue().getInt()));
    NodeValue current;
    switch(v.getObject())
    {
        /* 
         * This can go a couple of different ways:
         *
         *  append widget IDENTIFIER to self
         *      We will be creating a new widget as a child of 'self'
         *  
         *  append VALUE to widget own ATTRIBUTE
         */
        case MoguSyntax::own:{

            // Appends a new widget to the broadcaster widget's tree.
            if ((MoguSyntax) v.getArg() == MoguSyntax::widget)
            {
                mApp;
                std::string new_node = (std::string) v.getValue();
                const MoldableFactory& factory = app->getFactory();
                Moldable* child = factory.createMoldableWidget(new_node);
                MoldableAbstractParent* parent = (MoldableAbstractParent*) &broadcaster;
                parent->appendChild(child);
                return;
            }
            // Appends a value to the broadcaster's named attribute.
            broadcaster.getAttribute( (MoguSyntax) v.getArg(), current);
            if (current.getType() == ReadType::int_value)
                current.setString(std::to_string(current.getInt()));

            v.setAction(MoguSyntax::set);
            v.getValue().setString(
                v.getValue().getString() + current.getString());
            set(broadcaster,v);
            break;}

        case MoguSyntax::widget:{
            mApp;
            Moldable* widget = app->registeredWidget(v.getIdentifier());
            if ((MoguSyntax) v.getArg() == MoguSyntax::widget)
            {
                std::string new_node = (std::string) v.getValue();
                const MoldableFactory& factory = app->getFactory();
                MoldableAbstractParent* p_widget = (MoldableAbstractParent*) 
                    widget;
                Moldable* child = factory.createMoldableWidget(new_node);
                p_widget->appendChild(child);
                return;
            }
            widget->getAttribute( (MoguSyntax) v.getArg(), current);
            if (current.getType() == ReadType::int_value)
                current.setString(std::to_string(current.getInt()));
            v.getValue().setString(
                v.getValue().getString() + current.getString());
            v.setAction(MoguSyntax::set);
            set(broadcaster,v);
            break;}

        /* If the node is a list type, an entry will be
         * appended to the list. Otherwise, the new value will be 
         * appended to the current value at that node (or set if it isn't
         * already). 
         *
         * If you wish to append a value to a list ENTRY, you should set it
         * up like this (assuming you want to deal with 
         * index '1' in the example)
         *      set slot temp to user somelist 1
         *      append "new value" to slot temp
         *      set user somelist 1 to slot temp
         */
        case MoguSyntax::user:{
            mApp;
            std::string node =
                app->getUser()
                + std::string(".")
                + v.getIdentifier();

            Redis::ContextQuery db(Prefix::user);
            MoguSyntax node_type = getPolicyNodeType(v.getIdentifier());
            
            switch(node_type)
            {
                case MoguSyntax::list:
                    db.appendQuery("rpush user.%s %s",
                            node.c_str(), v.getValue().getString().c_str());
                    db.execute();
                    return;
                default: return;
            }

            Redis::addQuery(
                db
                , MoguSyntax::get
                , Prefix::user
                , node_type
                , v);
            v.getValue().setString(
                v.getValue().getString() + db.yieldResponse <std::string>());
            set(broadcaster,v);
            break;}

        case MoguSyntax::group:{
            mApp;
            std::string node = app->getGroup() + "." + v.getIdentifier();
            Redis::ContextQuery db(Prefix::group);

            MoguSyntax node_type = getPolicyNodeType(v.getIdentifier());

            switch(node_type)
            {
                case MoguSyntax::list:
                    db.appendQuery("rpush group.%s %s", node.c_str(),
                            v.getValue().getString().c_str());
                    db.execute();
                    return;
                default: return;
            }

            Redis::addQuery(
                db
                , MoguSyntax::get
                , Prefix::group
                , node_type
                , v);
            v.getValue().setString(
                v.getValue().getString() + db.yieldResponse <std::string>());
            set(broadcaster,v);
            break;}

        /* Similar to user and group contexts, if the destination node is
         * a 'list' type, the entry will default be appended to the list. */
        case MoguSyntax::data:{
            Redis::ContextQuery db(Prefix::data);
            db.appendQuery("type data.%s", v.getIdentifier().c_str());
            MoguSyntax storage_type = db.yieldResponse <MoguSyntax>();
            MoguSyntax node_type;
            switch(storage_type)
            {
                case MoguSyntax::list:
                    db.appendQuery("rpush data.%s %s",
                        v.getIdentifier().c_str()
                        , v.getValue().getString().c_str());
                    return;
                default:return;
            }
            Redis::addQuery(
                db
                , MoguSyntax::get
                , Prefix::data
                , node_type
                , v);
            v.getValue().setString(
                v.getValue().getString() + db.yieldResponse <std::string>());
            set(broadcaster,v);
            break;}

        case MoguSyntax::slot:{
            mApp;
            current = app->slotManager().retrieveSlot(v.getIdentifier());
            current.setString(
                current.getString() + v.getValue().getString());
            app->slotManager().setSlot(v.getIdentifier(), v.getValue());
            break;}

        case MoguSyntax::app:{
            if ((MoguSyntax)v.getValue() != MoguSyntax::user) return;
            mApp;
            app->getUserManager().registerUser(
                    app->slotManager().retrieveSlot("USERNAME"),
                    app->slotManager().retrieveSlot("PASSWORD")
            );
            break;}
        default:return;
    }
}

void reset(Moldable& broadcaster, CommandValue& v)
{
    if (v.getObject() == MoguSyntax::own)
    {
        broadcaster.reset();
    }
    else if (v.getObject() == MoguSyntax::widget)
    {
        mApp;
        Moldable* widget = app->registeredWidget(v.getIdentifier());
        widget->reset();
    }
}

void remove (Moldable& broadcaster, CommandValue& v)
{
    if (v.getValue().getType() == ReadType::int_value)
        v.getValue().setString(std::to_string(v.getValue().getInt()));
    
    switch(v.getObject())
    {
        case MoguSyntax::own:{
            MoguSyntax attribute = (MoguSyntax) v.getArg();
            switch(attribute)
            {
                // remove "foo" from own css
                // remove own css
                case MoguSyntax::style:
                    if (v.getValue().getString() == EMPTY)
                    {
                        broadcaster.setStyleClass("");
                        return;
                    }
                    broadcaster.removeStyleClass(v.getValue().getString());
                    break;
                case MoguSyntax::text:
                    broadcaster.setMoldableValue("");
                    break;
                case MoguSyntax::index:{
                    NodeValue tmp(0);
                    broadcaster.setAttribute(attribute, tmp);}
                    break;
                default: return;
            }
            break;}
                    
        case MoguSyntax::widget:{
            mApp;
            MoguSyntax attribute = (MoguSyntax) v.getArg();
            Moldable* widget = app->registeredWidget(v.getIdentifier());
            switch(attribute)
            {
                case MoguSyntax::style:
                    if (v.getValue().getString() == EMPTY)
                    {
                        broadcaster.setStyleClass("");
                        return;
                    }
                    widget->removeStyleClass(v.getValue().getString());
                    break;
                case MoguSyntax::text:
                    widget->setMoldableValue("");
                    break;
                case MoguSyntax::index:{
                    NodeValue tmp(0);
                    widget->setAttribute(attribute, tmp);
                    break;
                }
                case MoguSyntax::widget:{//remove widget foo from widget bar
                    Moldable* child = app->registeredWidget(
                            v.getValue().getString());
                    widget->removeChild(child);
                    break;}
                default:return;
            }
            break;}
        /* remove "an item" from user someList"
         * remove user someString
         * remove user someHash someField
         */
        case MoguSyntax::user:{
            mApp;
            std::string node =
                app->getUser()
                + std::string(".")
                + v.getIdentifier();
            Redis::ContextQuery policydb(Prefix::policies);
            Redis::ContextQuery db(Prefix::user);

            MoguSyntax node_type = getPolicyNodeType(v.getIdentifier());

            bool del_node = false;
            switch(node_type)
            {
                case MoguSyntax::list:
                    if (v.getValue().getString() != EMPTY)
                    {
                        db.appendQuery("ldel user.%s 1 %s"
                                , node.c_str()
                                , v.getValue().getString().c_str());
                        db.execute();
                        return;
                    }
                    else del_node = true;
                    break;
                case MoguSyntax::hash:
                    if (v.getArg().getString() != EMPTY)
                    {
                        db.appendQuery("hdel user.%s %s"
                                , node.c_str()
                                , v.getArg().getString().c_str());
                        db.execute();
                        return;
                    }
                    else del_node = true;
                    break;
                case MoguSyntax::string:
                    del_node = true;
                    break;                    
                default:return;
            }

            if (del_node)
            {
                db.appendQuery("del user.%s", node.c_str());
                return;
            }
            break;}

        case MoguSyntax::group:{
            mApp;
            std::string group = std::to_string (app->getGroup());
            std::string node = group + "." + v.getIdentifier();
            Redis::ContextQuery db(Prefix::group);

            MoguSyntax node_type = getPolicyNodeType(v.getIdentifier());
            
            bool del_node = false;

            switch(node_type)
            {
                case MoguSyntax::list:
                    if (v.getValue().getString() != EMPTY)
                    {
                        db.appendQuery("ldel group.%s 1 %s"
                                , node.c_str()
                                , v.getValue().getString().c_str());
                        db.execute();
                        return;
                    }
                    else del_node = true;
                    break;
                case MoguSyntax::hash:
                    if (v.getArg().getString() != EMPTY)
                    {
                        db.appendQuery("hdel group.%s %s"
                                , node.c_str()
                                , v.getArg().getString().c_str());
                        db.execute();
                        return;
                    }
                    else del_node = true;
                    break;
                case MoguSyntax::string:
                    del_node = true;
                    break;                    

                default: return;
            }

            if (del_node)
            {
                db.appendQuery("del group.%s", node.c_str());
                db.execute();
                return;
            }
            break;}

        // remove data someList someValue
        // remove data someHash someField
        // remove data someNode
        case MoguSyntax::data:{
            Redis::ContextQuery db(Prefix::data);

            db.appendQuery("type data.%s", v.getIdentifier().c_str());
            MoguSyntax node_type = db.yieldResponse<MoguSyntax>();
            bool del_node = false;
            switch(node_type)
            {
                case MoguSyntax::list:
                    if (v.getArg().getString() != EMPTY) 
                    {
                        db.appendQuery(
                            "ldel data.%s 1 %s"
                            , v.getIdentifier().c_str()
                            , v.getArg().getString().c_str());
                        db.execute();
                        return;
                    } else del_node = true;
                    break;
                case MoguSyntax::hash:
                    if (v.getArg().getString() != EMPTY)
                    {
                        db.appendQuery(
                            "hdel data.%s %s"
                            , v.getIdentifier().c_str()
                            , v.getArg().getString().c_str());
                        db.execute();
                        return;
                    } else del_node = true;
                    break;
                case MoguSyntax::string:
                    del_node = true;
                    break;
                default: return;
            }
            if (del_node)
            {
                db.appendQuery("del data.%s", v.getIdentifier().c_str());
                db.execute();
            }

            break;}

        // remove slot foo
        case MoguSyntax::slot:{
            mApp;
            app->slotManager().retrieveSlot(v.getIdentifier());
            break;}
        default: return;
        }
}

void javascript(Moldable& broadcaster, CommandValue& v)
{
    mApp;
    app->doJavaScript(v.getValue().getString());
}


void email(Moldable& broadcaster, CommandValue& v)
{
    mApp;
    NodeValue message;
    EmailManager email;
    email.setRecipient(v.getValue().getString());
    email.setSubject(app->slotManager().peekSlot("EMAIL_SUBJECT"));

    switch(v.getObject())
    {
        case MoguSyntax::own:{
            broadcaster.getAttribute( (MoguSyntax) v.getArg(), message);
            email.setMessage(message.getString());
            break;}

        case MoguSyntax::widget:{
            Moldable* widget = app->registeredWidget(v.getIdentifier());
            widget->getAttribute( (MoguSyntax) v.getArg(), message);
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

}//namespace Actions
