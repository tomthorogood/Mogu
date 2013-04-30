/*
 * Actions.cpp
 *
 *  Created on: Apr 22, 2013
 *      Author: tom
 */

#include <Mogu.h>
#include "Actions.h"


Redis::NodeType getPolicyNodeType(const std::string& identifier)
{
    Redis::ContextQuery policydb(Prefix::policies);
    CreateQuery(policydb,
        new Redis::Query("hget policies.%s %d",
            identifier.c_str(), MoguSyntax::type));
    return (MoguSyntax) atoi(policydb.yieldResponse <std::string>().c_str());
}

void policyQuery(
        MoguSyntax queryType
        , Redis::ContextQuery& db
        , Prefix prefix, CommandValue& v
        , init = true)
{
    mApp;

    if (!init) goto lbl_append_query
    MoguSyntax node_type = getPolicyNodeType(v.getIdenitifer());
    switch(prefix)
    {
        case Prefix::user:
            v.setIdentifier(app->getUser() + "." + v.getIdentifier());
            break;
        case Prefix::group:
            v.setIdentifier(app->getGroup() + "." + v.getIdentifier());
            break;
        default:return;
    }
lbl_append_query:
    Redis::appendQuery(
        db
        , queryType
        , prefix
        , node_type
        , v);
}

void set (Moldable& broadcaster, CommandValue& v)
{
    switch(v.getObject())
    {
        case MoguSyntax::own:   // set attribute of calling widget
            MoguSyntax attr = (MoguSyntax) v.getArg().getInt();
            broadcaster.setAttribute(attr,v.getValue());
            break;
        case MoguSyntax::path:  // set application URL
            mApp;
            app->setPath(v.getValue().getString());
            break;
        case MoguSyntax::user:  // set user field
            mApp;
            if (v.getIdentifier() == EMPTY)
            {
                app->userManager().loginUser();
                return;
            }
            Redis::ContextQuery db(Prefix::user);
            policyQuery(
                    MoguSyntax::set
                    , db
                    , Prefix::user
                    , v);
            db.execute();
            break;
        case MoguSyntax::group: // set group field
            mApp;
            if (v.getIdentifier() == EMPTY)
            {
                app->userManager().setActiveGroup(v.getValue().getString());
                return;
            }
            Redis::ContextQuery db(Prefix::group);
            policyQuery(
                MoguSyntax::set
                , db
                , Prefix::group
                , v);
            db.execute();
            break;
        case MoguSyntax::data:  // set data field
            Redis::ContextQuery db(Prefix::data);
            CreateQuery(db,
                new Redis::Query("type data.%s"), 
                v.getIdentifier().c_str());
            NodeType node_type = nodeTypeMap.at(
                    policydb.yieldResponse<std::string>());
            Redis::addQuery(
                db
                , Redis::CommandType::set
                , Prefix::data
                , node_type
                , v);
            db.execute();                    
            break;
        case MoguSyntax::slot:  // set application slot
            mApp;
            app->slotManager().setSlot(v.getIdentifier(), v.getValue());
            break;
        case MoguSyntax::widget:// set arbitrary widget attribute
            mApp;
            Moldable* widget = app->registeredWidget(v.getIdentifier());
            widget->setAttribute(v.getArg().getInt(), v.getValue());
        default: return; // If bad input, stop immediately
    }
}

void increment (Moldable& broadcaster, CommandValue& v)
{
    int value = (v.getFlags() & v.VALUE) ?
        v.getValue().getInt() : 1;
    switch(v.getObject())
    {
        case MoguSyntax::own:
            NodeValue current;
            broadcaster.getAttribute( (MoguSyntax) v.getArg(), current);
            if (current.getType() == string_type)
                current.setInt(atoi(current.getString().c_str()));
            current.setInt(current.getInt()+val);
            broadcaster.setAttribute( (MoguSyntax) v.getArg(), current);
            break;

        case MoguSyntax::user:
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
            break;

        case MoguSyntax::group:
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
            break;

        case MoguSyntax::data:
            Redis::ContextQuery(Prefix::data);
            NodeValue final(value);
            v.setValue(final);
            Redis::addQuery(
                db
                , Redis::CommandType::get
                , Prefix::data
                , node_type
                , v);
            Redis::addQuery(
                db
                , Redis::CommandType::set
                , Prefix::data
                , node_type
                , v);
            final.setInt(
                final.getInt() + atoi(db.yieldResponse<std::string>().c_str())
            );
            db.execute();

            break;
        case MoguSyntax::slot:
            mApp;
            NodeValue& final = app->slotManager().retrieveSlot(
                    v.getIdentifier());
            final.setInt(final.getInt()+value);
            app->slotManager().setSlot(
                    v.getIdentifier(), final);
            break;

        case MoguSyntax::widget:
            mApp;
            Moldable* widget = app->registeredWidget(v.getIdentifier());
            NodeValue current;
            widget->getAttibute( (MoguSyntax) v.getArg(), current);
            if (current.getType() == string_type)
                current.setInt(atoi(current.getString().c_str()));
            current.setInt(current.getInt()+val);
            widget->setAttribute( (MoguSyntax) v.getArg(), current );
            break;
        default: return;
    }
} 

void decrement (Moldable& broadcaster, CommandValue& v)
{
    int value = (v.getFlags() & v.VALUE) ?
        v.getValue().getInt() : 1;
    switch(v.getObject())
    {
        case MoguSyntax::own:
            NodeValue current;
            broadcaster.getAttribute( (MoguSyntax) v.getArg(), current);
            if (current.getType() == string_type)
                current.setInt(atoi(current.getString().c_str()));
            current.setInt(current.getInt()-val);
            broadcaster.setAttribute( (MoguSyntax) v.getArg(), current);
            break;

        case MoguSyntax::user:
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
            break;

        case MoguSyntax::group:
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
            break;

        case MoguSyntax::data:
            Redis::ContextQuery(Prefix::data);
            NodeValue final(value);
            v.setValue(final);
            Redis::addQuery(
                db
                , Redis::CommandType::get
                , Prefix::data
                , node_type
                , v);
            Redis::addQuery(
                db
                , Redis::CommandType::set
                , Prefix::data
                , node_type
                , v);
            final.setInt(
                final.getInt() - atoi(db.yieldResponse<std::string>().c_str())
            );
            db.execute();

            break;

        case MoguSyntax::slot:
            mApp;
            NodeValue& final = app->slotManager().retrieveSlot(
                    v.getIdentifier());
            final.setInt(final.getInt()-value);
            app->slotManager().setSlot(
                    v.getIdentifier(), final);
            break;

        case MoguSyntax::widget:
            mApp;
            Moldable* widget = app->registeredWidget(v.getIdentifier());
            NodeValue current;
            widget->getAttibute( (MoguSyntax) v.getArg(), current);
            if (current.getType() == string_type)
                current.setInt(atoi(current.getString().c_str()));
            current.setInt(current.getInt()-val);
            widget->setAttribute( (MoguSyntax) v.getArg(), current );
            break;
        default: return;
    }
} 

void test(Moldable& broadcaster, CommandValue& v)
{
    bool result = false;
    NodeValue value;
    // Here we will be testing to see whether the 
    // value in the first clause ('object') matches
    // the value in the second clause ('value'). This one is 
    // a bit odd, admittedly. There's probably a much better way to handle it.

    switch(v.getObject())
    {
        case MoguSyntax::own:
            broadcaster.getAttribute( (MoguSyntax) v.getArg().getInt(), value);
            break;
        case MoguSyntax::user:
            Redis::ContextQuery db(Prefix::user);
            policyQuery(
                MoguSyntax::get
                , db
                , Prefix::user
                ,v);
            value.setString(db.yieldResponse<std::string>()); 
            break;

        case MoguSyntax::group:
            Redis::ContextQuery db(Prefix::group);
            policyQuery(
                MoguSyntax::get
                , db
                , Prefix::group
                ,v);
            value.setString(db.yieldResponse<std::string>()); 
            break;

        case MoguSyntax::data:
            ContextQuery db(Prefix::data);
            CreateQuery(db
                new Redis::Query("type data.%s", v.getIdentifier()));
            Redis::addQuery(
                db
                , Redis::CommandType::get
                , Prefix::data
                , Redis::nodeTypeMap(db.yieldResponse <std::string>())
                , v);
            value.setString(db.yieldResponse <std::string>());
            break;
        case MoguSyntax::slot:
            mApp;
            value = app->slotManager().peekSlot(v.getIdentifier());
            break;
        case MoguSyntax::widget:
            mApp;
            Moldable* widget = app->registeredWidget(v.getIdentifier());
            widget->getAttribute( (MoguSyntax) v.getArg(), value)
            break;
        default: return;
    }
    result = (value == v.getValue());
    if (!result) broadcaster.fail().emit();
    else broadcaster.succeed().emit();
}

void reload(Moldable& broadcaster, CommandValue& v)
{
    switch(v.getObject())
    {
        case MoguSyntax::own:
            broadcaster.reload();
            break;
        case MoguSyntax::widget:
            mApp;
            Moldable* widget = app->registeredWidget(v.getIdentifier());
            widget->reload();
            break;
        default: return;
    }
}

void append(Moldable& broadcaster, CommandValue& v)
{
    if (v.getValue().getType() == integer_type)
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
        case MoguSyntax::own:

            // Appends a new widget to the broadcaster widget's tree.
            if ((MoguSyntax) v.getArg().getInt() == MoguSyntax::widget)
            {
                mApp;
                std::string new_node = v.getValue().getString();
                const MoldableFactory& factory = app->getFactory();
                const AbstractMoldableChild& = 
                    (const AbstractMoldableChild&) broadcaster;
                Moldable* child = factory.createMoldableWidget(new_node);
                broadcaster.addChild(child);
                return;
            }
            // Appends a value to the broadcaster's named attribute.
            broadcaster.getAttribute( 
                    (MoguSyntax) v.getArg().getInt(), current);
            if (current.getType() == integer_type)
                current.setString(std::to_string(current.getInt()));

            v.setAction(MoguSyntax::set);
            v.getValue().setString(
                v.getValue().getString() + current.getString());
            set(broadcaster,v);
            break;
        case MoguSyntax::widget:
            mApp;
            Moldable* widget = app->registeredWidget(v.getIdentifier());
            if ((MoguSyntax) v.getArg().getInt() == MoguSyntax::widget)
            {
                std::string new_node = v.getValue().getString();
                const MoldableFactor& factory = app->getFactory();
                AbstractMoldableChild* p_widget = (AbstractMoldableChild*) 
                    widget;
                Moldable* child = factory.createMoldableWidget(new_node);
                p_widget->addChild(child);
                return;
            }
            widget->getAttibute(
                    (MoguSyntax) v.getArg().getInt(), current);
            if (current.getType() == integer_type)
                current.setString(std::to_string(current.getInt()));
            v.getValue().setString(
                v.getValue().getString() + current.getString());
            v.setAction(MoguSyntax::set);
            set(broadcaster,v);
            break;

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
        case MoguSyntax::user:
            mApp;
            std::string node = app.getUser() + "." + v.getIdentifier();
            Redis::ContextQuery policydb(Prefix::policies);
            Redis::ContextQuery db(Prefix::user);
            Redis::NodeType node_type = getPolicyNodeType(v.getIdentifier());
            
            switch(storage_type)
            {
                case MoguSyntax::list:
                    CreateQuery(db,
                        "rpush user.%s %s", node.c_str(), 
                        v.getValue().getString().c_str());
                    db.execute();
                    return;
                case MoguSyntax::hash:
                    node_type = Redis::nodeTypeMap.at("hash");
                    break;
                case MoguSyntax::string:
                    node_type = Redis::nodeTypeMap.at("string");
            }

            Redis::addQuery(
                db
                , Redis::CommandType::get
                , Prefix::user
                , node_type
                , v);
            v.getValue().setString(
                v.getValue().getString() + db.yieldResponse <std::string>());
            set(broadcaster,v);
            break;
        case MoguSyntax::group:
            mApp;
            std::string node = app.getGroup() + "." + v.getIdentifier();
            Redis::ContextQuery db(Prefix::group);

            Redis::NodeType node_type = getPolicyNodeType(v.getIdentifier());

            switch(storage_type)
            {
                case MoguSyntax::list:
                    CreateQuery(db,
                        "rpush group.%s %s", node.c_str(), 
                        v.getValue().getString().c_str());
                    db.execute();
                    return;
                case MoguSyntax::hash:
                    node_type = Redis::nodeTypeMap.at("hash");
                    break;
                case MoguSyntax::string:
                    node_type = Redis::nodeTypeMap.at("string");
            }

            Redis::addQuery(
                db
                , Redis::CommandType::get
                , Prefix::group
                , node_type
                , v);
            v.getValue().setString(
                v.getValue().getString() + db.yieldResponse <std::string>());
            set(broadcaster,v);
            break;

        /* Similar to user and group contexts, if the destination node is
         * a 'list' type, the entry will default be appended to the list. */
        case MoguSyntax::data:
            Redis::ContextQuery(Prefix::data);
            CreateQuery(db,
                new Redis::Query("type data.%s", v.getIdentifier().c_str()));
            MoguSyntax storage_type = (MoguSyntax)
                atoi(db.yieldResponse<std::string>).c_Str();
            Redis::NodeType node_type;
            switch(storage_type)
            {
                case MoguSyntax::list:
                    CreateQuery(db,
                        new Redis::Query("rpush data.%s %s", 
                            v.getIdentifier().c_str()
                            , v.getValue().getString()));
                    return;
                case MoguSyntax::hash:
                    node_type = Redis::nodeTypeMap.at("hash");
                    break;
                case MoguSyntax::string:
                    node_type = Redis::nodeTypeMap.at("string");
                    break;
                default return;
            }
            Redis::addQuery(
                db
                , Redis::CommandType::get
                , Prefix::data
                , node_type
                , v);
            v.getValue().setString(
                v.getValue().getString() + db.yieldResponse <std::string>());
            set(broadcaster,v);
            break
        case MoguSyntax::slot:
            mApp;
            current = app->slotManager().retrieveSlot(v.getIdentifier());
            current.setString(
                current.getString() + v.getValue().getString());
            app->slotManager().setSlot(v.getIdentifier(), value);
            break;

        case MoguSyntax::app:
            if (v.getValue().getInt() != (int) MoguSyntax::user) return;
            mApp;
            SlotManager& slots = app->slotManager();
            app->userManager().registerUser(
                    slots.retrieveSlot("USERNAME"),
                    slots.retrieveSlot("PASSWORD")
            );
            break;

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
    if (v.getValue().getType() == integer_type)
        v.getValue().setString(std::to_string(v.getValue().getInt()));
    
    switch(v.getObject())
    {
        case MoguSyntax::self:
            MoguSyntax attribute = (MoguSyntax) v.getArg().getInt();
            switch(attribute)
            {
                // remove "foo" from own css
                // remove own css
                case MoguSyntax::css:
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
                case MoguSyntax::index:
                    broadcaster.setAttribute(attribute, NodeValue(0));
                default: return;
            }
                    
        case MoguSyntax::widget:
            mApp;
            MoguSyntax attribute = (MoguSyntax) v.getArg().getInt();
            Moldable* widget = app->registeredWidget(v.getIdentifier());
            switch(attribute)
            {
                case MoguSyntax::css:
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
                case MoguSyntax::index:
                    widget->setAttribute(attribute, NodeValue(0));
                case MoguSyntax::widget: //remove widget foo from widget bar
                    Moldable* child = app->registeredWidget(v.getValue().getString());
                    widget->removeChild(child);
                default:return;
            }

        /* remove "an item" from user someList"
         * remove user someString
         * remove user someHash someField
         */
        case MoguSyntax::user:
            mApp;
            std::string node = app->getUser() + "." + v.getIdentifier();
            Redis::ContextQuery policydb(Prefix::policies);
            Redis::ContextQuery db(Prefix::user);

            Redis::NodeType node_type = getPolicyNodeType(v.getIdentifier());

            bool del_node = false;
            switch(storage_type)
            {
                case MoguSyntax::list:
                    if (v.getValue().getString() != EMPTY)
                    {
                        CreateQuery(db,
                            new Redis::Query("ldel user.%s 1 %s"
                                , node.c_str()
                                , v.getValue().getString().c_str()));
                        return;
                    }
                    else del_node = true;
                    break;
                case MoguSyntax::hash:
                    if (v.getArg().getString() != EMPTY)
                    {
                        CreateQuery(db,
                            new Redis::Query("hdel user.%s %s"
                                , node.c_str()
                                , v.getArg().getString().c_str()));
                        return;
                    }
                    else del_node = true;
                    break;
                case MoguSyntax::string:
                    del_node = true;
                    break;                    
            }

            if (del_node)
            {
                CreateQuery(db,
                    new Redis::Query("del user.%s", node.c_str()));
                return;
            }
            break;
        case MoguSyntax::group:
            mApp;
            std::string node = app->getGroup() + "." + v.getIdentifier();
            Redis::ContextQuery db(Prefix::group);

            Redis::NodeType node_type = getPolicyNodeType(v.getIdentifier());
            
            bool del_node = false;

            switch(storage_type)
            {
                case MoguSyntax::list:
                    if (v.getValue().getString() != EMPTY)
                    {
                        CreateQuery(db,
                            new Redis::Query("ldel group.%s 1 %s"
                                , node.c_str()
                                , v.getValue().getString().c_str()));
                        return;
                    }
                    else del_node = true;
                    break;
                case MoguSyntax::hash:
                    if (v.getArg().getString() != EMPTY)
                    {
                        CreateQuery(db,
                            new Redis::Query("hdel group.%s %s"
                                , node.c_str()
                                , v.getArg().getString().c_str()));
                        return;
                    }
                    else del_node = true;
                    break;
                case MoguSyntax::string:
                    del_node = true;
                    break;                    
            }

            if (del_node)
            {
                CreateQuery(db,
                    new Redis::Query("del group.%s", node.c_str()));
                return;
            }
            break;

        // remove data someList someValue
        // remove data someHash someField
        // remove data someNode
        case MoguSyntax::data:
            Redis::ContextQuery db(Prefix::data);

            CreateQuery(db,
                new Redis::Query("type data.%s", v.getIdentifier().c_str()));
            MoguSyntax node_type = (MoguSyntax)
                atoi(db.yieldResponse <std::string>().c_str());
            bool del_node = false;
            switch(node_type)
            {
                case MoguSyntax::list:
                    if (v.getArg().getString() != EMPTY) 
                    {
                        CreateQuery(db,
                            new Redis::Query(
                                "ldel data.%s 1 %s"
                                , v.getIdentifier().c_str()
                                , v.getArg().getString().c_str()));
                        db.execute();
                        return;
                    } else del_node = true;
                    break;
                case MoguSyntax::hash:
                    if (v.getArg().getString() != EMPTY)
                    {
                        CreateQuery(db,
                            new Redis::Query(
                                "hdel data.%s %s"
                                , v.getIdentifier().c_str()
                                , v.getArg().getString().c_str()));
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
                CreateQuery(db
                    new Redis::Query("del data.%s", v.getIdentifier().c_str()));
                db.execute();
            }

            break;

        // remove slot foo
        case MoguSyntax::slot:
            mApp;
            app->slotManager().retrieveSlot(v.getIdentifier());
            break;
        }
}

void javascript(Moldable& broadcaster, CommandValue& v)
{
    mApp;
    app->doJavascript(v.getValue().getString());
}


void email(Moldable& broadcaster, CommandValue& v)
{
    mApp;
    NodeValue message;
    EmailManager email;
    email.setRecipient(v.getValue.getString());
    email.setSubject(app->slotManager().peekSlot("EMAIL_SUBJECT"));

    switch(v.getObject())
    {
        case MoguSyntax::own:
            broadcaster.getAttribute(
                    (MoguSyntax) v.getArg().getInt(), message);
            email.setMessage(message.getString());
            break;

        case MoguSyntax::widget:
            Moldable* widget = app->registeredWidget(v.getIdentifier()).get();
            widget->getAttribute(
                    (MoguSyntax) v.getArg().getInt(),n message);
            email.setMessage(message.getString());

        case MoguSyntax::user:
            Redis::ContextQuery db(Prefix::user);
            policyQuery(
                MoguSyntax::get
                , db
                , Prefix::user
                , v);
            message.setString(db.yieldResponse<std::string>);
            break;
        case MoguSyntax::group:
            Redis::ContextQuery db(Prefix::group);
            policyQuery(
                MoguSyntax::get
                , db
                , Prefix::group
                , v);
            message.setString(db.yieldResponse<std::string>);
            break;

        case MoguSyntax::slot:
            message.setString(app->slotManager().retrieveSlot(
                    v.getIdentifier()));
            break;

        case MoguSyntax::data:
            Redis::ContextQuery db(Prefix::data);
            CreateQuery(db,
                new Redis::Query("type data.%s", v.getIdentifier()).c_str());
            Redis::NodeType node_type = 
                Redis::nodeTypeMap.at(
                    db.yieldResponse <std::string>());
            Redis::appendQuery(
                db
                , Redis::CommandType::get
                , Prefix::data
                , node_type
                , v);
            message.setString(db.yieldResponse <std::string>());
            break;
        default: return;
    }
    email.setMessage(message.getString());
    email.send();
}
