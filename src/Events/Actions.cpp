/*
 * Actions.cpp
 *
 *  Created on: Apr 22, 2013
 *      Author: tom
 */

#include <Mogu.h>
#include "Actions.h"

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
            std::string node = app->getUser() + "." + v.getIdentifier();
            Redis::ContextQuery policydb(Prefix::policies);
            CreateQuery(policydb,
                new Redis::Query("hget policies.%s %s",
                   v.getIdentifier().c_str(), "type"));
            NodeType node_type = nodeTypeMap.at(
                    policydb.yieldResponse<std::string>());
            // Rewrite the node in the proper 
            // [userid].field_name format
            v.setIdentifier(node);
            Redis::ContextQuery db(Prefix::user);
            Redis::addQuery(                            
                db
                , Redis::CommandType::set
                , Prefix::user
                , node_type
                , v); 

            db.execute();
            break;
        case MoguSyntax::group: // set group field
            mApp;
            std::string node = app->getGroup() + 
                "." + v.getIdentifier();
            Redis::ContextQuery policydb(Prefix::policies);
            CreateQuery(policydb,
                new Redis::Query("hget policies.%s %s",
                    v.getIdentifier().c_str(), "type"));
            NodeType node_type = nodeTypeMap.at(
                    policydb.yieldResponse<std::string>());
            v.setIdentifier(node);
            Redis::ContextQuery db(Prefix::group);
            Redis::addQuery(
                db
                , Redis::CommandType::set
                , Prefix::group
                , node_type
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
            mApp;
            std::string node = app->getUser() + "." + v.getIdentifier();
            Redis::ContextQuery policydb(Prefix::policies);
            Redis::ContextQuery db(Prefix::user);

            CreateQuery(policydb,
                new Redis::Query("hget policies.%s %s",
                    v.getIdentifier().c_str(), "type"));
    
            v.setIdentifier(node);
            Redis::NodeType node_type = Redis::nodeTypeMap.at(
                    policydb.yieldResponse<std::string>());
            
            NodeValue final(value);
            v.setValue(final);
            Redis::addQuery(
               db
               , Redis::CommandType::get
               , Prefix::user
               , node_type
               , v);
            Redis::addQuery(
               db
               , Redis::CommandType::set
               , Prefix::user
               , node_type
               , v);
            final.setInt(
                final.getInt() + atoi(db.yieldResponse<std::string>().c_str())
            );
            db.execute();
            break;
        case MoguSyntax::group:
            mApp;
            std::string node = app->getGroup() + "." + v.getIdentifier();
            Redis::ContextQuery policydb(Prefix::policies);
            Redis::ContextQuery db(Prefix::group);
            CreateQuery(policydb,
                new Redis::Query("hget policies.%s %s",
                    v.getIdentifier().c_str(), "type"));
            v.setIdentifier(node);
            Redis::NodeType node_type = Redis::nodeTypeMap.at(
                policydb.yieldResponse<std::string>());
            NodeValue final(value);
            v.setValue(final);
            Redis::addQuery(
                db
                , Redis::CommandType::get
                , Prefix::group
                , node_type
                , v);
            Redis::addQuery(
                db
                , Redis::CommandType::set
                , Prefix::group
                , node_type
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
            mApp;
            std::string node = app->getUser() + "." + v.getIdentifier();
            Redis::ContextQuery policydb(Prefix::policies);
            Redis::ContextQuery db(Prefix::user);

            CreateQuery(policydb,
                new Redis::Query("hget policies.%s %s",
                    v.getIdentifier().c_str(), "type"));
    
            v.setIdentifier(node);
            Redis::NodeType node_type = Redis::nodeTypeMap.at(
                    policydb.yieldResponse<std::string>());
            
            NodeValue final(value);
            v.setValue(final);
            Redis::addQuery(
               db
               , Redis::CommandType::get
               , Prefix::user
               , node_type
               , v);
            Redis::addQuery(
               db
               , Redis::CommandType::set
               , Prefix::user
               , node_type
               , v);
            final.setInt(
                final.getInt() - atoi(db.yieldResponse<std::string>().c_str())
            );
            db.execute();
            break;
        case MoguSyntax::group:
            mApp;
            std::string node = app->getGroup() + "." + v.getIdentifier();
            Redis::ContextQuery policydb(Prefix::policies);
            Redis::ContextQuery db(Prefix::group);
            CreateQuery(policydb,
                new Redis::Query("hget policies.%s %s",
                    v.getIdentifier().c_str(), "type"));
            v.setIdentifier(node);
            Redis::NodeType node_type = Redis::nodeTypeMap.at(
                policydb.yieldResponse<std::string>());
            NodeValue final(value);
            v.setValue(final);
            Redis::addQuery(
                db
                , Redis::CommandType::get
                , Prefix::group
                , node_type
                , v);
            Redis::addQuery(
                db
                , Redis::CommandType::set
                , Prefix::group
                , node_type
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
            mApp;
            std::string node = app->getUser() + "." + v.getIdentifier();

            ContextQuery policydb(Prefix::policies);
            ContextQuery db(Prefix::user);

            CreateQuery(policydb,
                new Redis::Query("hget policies.%s %s",
                    v.getIdentifier().c_str(), "type"));
            
            if (v.getValue().getType() == integer_type)
                v.getValue().setString(std::to_string(v.getValue().getInt()));
            v.setIdentifier(node);
            Redis::NodeType node_type = Redis::nodeTypeMap.at(
                    policydb.yieldResponse <std::string>());
            Redis::addQuery(
                db
                , Redis::CommandType::get
                , Prefix::user
                , node_type
                , v);
            value.setString(db.yieldResponse<std::string>()); 
            break;
        case MoguSyntax::group:
            mApp;
            std::string node = app->getGroup() + "." + v.getIdentifier();

            ContextQuery policydb(Prefix::policies);
            ContextQuery db(Prefix::user);

            CreateQuery(policydb,
                new Redis::Query("hget policies.%s %s",
                    v.getIdentifier().c_str(), "type"));
            v.setIdentifier(node);
            if (v.getValue().getType() == integer_type)
                v.getValue().setString(std::to_string(v.getValue().getInt()));
            Redis::NodeType node_type = Redis::nodeTypeMap.at(
                    policydb.yieldResponse <std::string>());
            Redis::addQuery(
                db
                , Redis::CommandType::get
                , Prefix::group
                , node_type
                , v);
            value.setString(db.yieldResponse <std::string>());
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
