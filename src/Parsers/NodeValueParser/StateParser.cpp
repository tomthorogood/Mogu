/*
 * StateParser.cpp
 *
 *  Created on: March 31th, 2013
 *      Author: cameron
 */

#include <Mogu.h>
#include <Parsers/NodeValueParser/StateParser.h>
#include <Redis/ContextQuery.h>
#include <Types/syntax.h>
#include <Redis/ContextQuery.h>
#include <Security/Encryption.h>
#include <Groups/GroupManager.h>

namespace Parsers {


StateParser::StateParser(TokenManager& tm) : __tm(tm)
{
}

/* In most circumstances, we will require an identifier to go along 
 * with an object. When required, this function will advance the 
 * TokenManager's pointer, assert that an Identifier was found, retrieve
 * the identifier (and return it), and advance the TokenManager to the next
 * pointer.
 */
std::string StateParser::getIdentifier()
{
    __tm.next();
    if (__tm.currentToken <MoguSyntax> () != MoguSyntax::TOKEN_DELIM)
    {
        //TODO throw a pretty big error, as this should only happen in the 
        //case of bad syntax checking on import, or database corruption.
    }
    std::string identifier = __tm.fetchStringToken();
    __tm.next();
    return identifier;
}

void StateParser::processInput(Moldable* broadcaster)
{
    __tm.saveLocation();
    MoguSyntax currentToken = __tm.currentToken <MoguSyntax>();
    NodeValue result;
    std::string identifier;

	switch(currentToken)
	{
		case MoguSyntax::widget:
            identifier = getIdentifier();
            handleWidget(identifier,result);
            break;
        case MoguSyntax::own:
            __tm.next();
            handleWidget(broadcaster, result);
            break;
		case MoguSyntax::data:
            identifier = getIdentifier();
            handleData(identifier, result);
            break;
		case MoguSyntax::user:
            identifier = getIdentifier();
            handleUserField(identifier, result);
            break;
		case MoguSyntax::group:
            identifier = getIdentifier();
            handleGroupField(identifier, result);
            break;
		case MoguSyntax::slot:{
            mApp;
            identifier = getIdentifier();
            result = app->slotManager().retrieveSlot(identifier);
            break;
          }
        default:{
            result.setInt((int)currentToken);
            break;
        }
	}

    __tm.deleteFromSaved();

    switch(result.getType())
    {
        case ReadType::string_value:
            __tm.injectToken(result.getString());
            break;
        case ReadType::int_value:
            __tm.injectToken(result.getInt());
            break;
        default:
            // We'll figure out floats or bad values later (TODO)
            __tm.injectToken(0); 
            break;
    }

}

// Handles the resolution of a named widget's states
void StateParser::handleWidget(const std::string& identifier, NodeValue& result) 
{
    mApp;
    Moldable* widget = app->registeredWidget(identifier);

    // For now, just ignore everything if the widget is not actually registered
    // due to premature deletion, or a mistyped name (which would hopefully be
    // caught on import).
    // TODO
    if (widget == nullptr) return; 
    
    // getIdentifier will have already advaned the token pointer 
    MoguSyntax widget_attribute = __tm.currentToken <MoguSyntax>();
    widget->getAttribute(widget_attribute, result);
}

void StateParser::handleData(const std::string& identifier, NodeValue& result)
{
    const char* c_node = identifier.c_str();
    Redis::ContextQuery db(Prefix::data);
    db.appendQuery( "type data.%s", c_node); 
    std::string node_type = db.yieldResponse <std::string>();
    if (node_type == "string")
    {
        //We need no further information!
        db.appendQuery( "get data.%s", c_node);
    }
    else if (node_type == "hash")
    {
        // If this is the case, the very next argument must inherently be
        // the hash key to resolve the field. There are two possibilities:
        MoguSyntax hashkey = __tm.currentToken <MoguSyntax> (); 

        // The key is a string, such as "foo", with syntax like: 
        //  hget data.bar foo 
        if (hashkey == MoguSyntax::TOKEN_DELIM)
            db.appendQuery( "hget data.%s %s", c_node,
                    __tm.fetchStringToken().c_str());

        // Or the key is a string that is really an integer, with 
        // syntax like:
        //
        // hget data.bar 17
        else
            db.appendQuery( "hget data.%s %d", c_node,
                    __tm.currentToken<int>());

        // NOTE that there is the possibility of a collision in the highly
        // unlikely case that the hash key integer value is the same as the 
        // TOKEN_DELIM integer. Again, though, this is super unlikely, but it
        // should be noted as a later TODO.
    }
    else if (node_type == "list")
    {
        // In this case, there must necessarily be a list index in order
        // to find the value.
        db.appendQuery( "lindex data.%s %d", c_node, __tm.currentToken<int>());

    }

    result.setString(db.yieldResponse<std::string>());
}

void StateParser::handleWidget(Moldable* widget, NodeValue& result)
{
    if (widget == nullptr) return;
    widget->getAttribute(__tm.currentToken <MoguSyntax>(), result);
}

void StateParser::handleUserField(const std::string& field, NodeValue& result)
{
    mApp;
    int userid = app->getUser();
    Redis::ContextQuery policies(Prefix::policies);
    Redis::ContextQuery user(Prefix::user);
    const char* c_field = field.c_str();
    MoguSyntax type;
    MoguSyntax token;

    // Determine the expected type and encryption
    // of the field in question:
    policies.appendQuery(
        "exists policies.%s.%d", c_field, MoguSyntax::default_);
    policies.appendQuery(
        "hget policies.%s %d", c_field, MoguSyntax::encrypted);
    
    user.appendQuery("hexists user.%d.%s", userid, c_field);
   
    bool has_default = policies.yieldResponse <bool>();
    
    // Avoids any unnecessary queries to the user database.
    bool user_has_field = 
       userid != -1 ? user.yieldResponse <bool>() : false;
    
    // Note that default values are never encrypted. This is to provide
    // an extra layer of security, because otherwise encrypted data would
    // have to reside in MoguScripts.
    bool encrypted = (policies.yieldResponse <std::string >() == "yes")
        && user_has_field;

    if (user_has_field) 
    {
        policies.appendQuery(
            "hget policies.%s %d", c_field, MoguSyntax::type);
        type = policies.yieldResponse <MoguSyntax>();
    }
    else if (has_default)
    {
        policies.appendQuery(
            "type policies.%s.%d", c_field, MoguSyntax::default_);
        std::string redistype = policies.yieldResponse <std::string>();
        type = 
            (redistype == "string") ? MoguSyntax::string :
            (redistype == "hash")   ? MoguSyntax::hash : MoguSyntax::list;
    }
    

    if (user_has_field)
    {
        switch(type)
        {
            case MoguSyntax::string:
                user.appendQuery( "get user.%d.%s", userid, c_field);
                break;
            case MoguSyntax::list:{
                int index = __tm.currentToken <int> ();
                user.appendQuery( "lindex user.%d.%s %d",
                       userid, c_field, index);
                break;}
            case MoguSyntax::hash:{
                std::string key;
                token = __tm.currentToken <MoguSyntax>();
                if (token == MoguSyntax::TOKEN_DELIM)
                    key = __tm.fetchStringToken();
                else
                    key = std::to_string(__tm.currentToken<int>());
                user.appendQuery("hget user.%d.%s %s",
                    userid, c_field, key.c_str());
                break;}
            default: return;
        };
    } else {
        switch(type)
        {
            case MoguSyntax::string:
                policies.appendQuery("get policies.%s.%d", c_field,
                    MoguSyntax::default_);
                break;
            case MoguSyntax::list:{
                int index = __tm.currentToken <int>();
                policies.appendQuery("lindex policies.%s.%d %d",
                        c_field, MoguSyntax::default_, index);
                }
                break;
            case MoguSyntax::hash:{
                std::string key;
                token = __tm.currentToken <MoguSyntax>();
                if (token == MoguSyntax::TOKEN_DELIM)
                    key = __tm.fetchStringToken();
                else
                    key = std::to_string((int) token);
                const char* c_key = key.c_str();
                policies.appendQuery("hget policies.%s.%d %s",
                    c_field, MoguSyntax::default_, c_key);
                break;}
            default: return;
        };
    }

    Redis::ContextQuery& final =
        user_has_field ? user : policies;

    // Now, we have to send the output through the NVP, in case it's a
    // recursive call:
    Parsers::NodeValueParser nvp;
    nvp.giveInput(final.yieldResponse <std::string>(), result);

    if (encrypted)
        result.setString(
                Security::decrypt( result.getString() ));
}

void StateParser::handleGroupField(const std::string& field, NodeValue& result)
{
    mApp;
    int group = app->getGroup();
    GroupManager group_mgr(group);
    bool read_enabled = group_mgr.hasReadAccess(field);
    if (!read_enabled) return;
    
    Redis::ContextQuery& grpdb = group_mgr.getContext(Prefix::group); 
    Redis::ContextQuery& plcdb = group_mgr.getContext(Prefix::policies);

    plcdb.appendQuery( "hget policies.%s %d", field.c_str(), MoguSyntax::type);
    plcdb.appendQuery( "hget policies.%s %d",
            field.c_str(), MoguSyntax::encrypted);
    MoguSyntax type = plcdb.yieldResponse <MoguSyntax>();
    bool encrypted = plcdb.yieldResponse <bool>();

    switch(type)
    {
        case MoguSyntax::string:
            grpdb.appendQuery( "get groups.%s.%s", group,
                    field.c_str());
            break;
        case MoguSyntax::list:{
            // Get the index of list item
            int index = __tm.currentToken <int>();
            grpdb.appendQuery( "lindex groups.%s.%s %d", group,
                    field.c_str(), index);
            break;}
        case MoguSyntax::hash:{
            // Ensure that the next token is in fact a key:
            if (__tm.currentToken<MoguSyntax>() != MoguSyntax::TOKEN_DELIM) return;
            std::string key = __tm.fetchStringToken();
            grpdb.appendQuery( "hget groups.%s.%s %s",
                    group, field.c_str(), key.c_str());
            break;}
        default: return;
    };

    if (encrypted)
        result.setString(
            Security::decrypt(
                grpdb.yieldResponse <std::string>()));
    else
        result.setString(grpdb.yieldResponse <std::string>());
}
}	// namespace Parsers
