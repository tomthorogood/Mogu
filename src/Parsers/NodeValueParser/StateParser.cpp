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
		case MoguSyntax::user:
            identifier = getIdentifier();
            handleUserField(identifier, result);
		case MoguSyntax::group:
            identifier = getIdentifier();
            handleGroupField(identifier, result);
		case MoguSyntax::slot:{
            mApp;
            identifier = getIdentifier();
            result = app->slotManager().retrieveSlot(identifier);
            break;
          }
        default:break;
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
    }

}

// Handles the resolution of a named widget's states
void StateParser::handleWidget(const std::string& identifier, NodeValue& result) 
{
    mApp;
    Moldable* widget = app->registeredWidget(identifier);

    // For now, just ignore everything if the widget is not actually registered
    // due to premature deletion, or a mistyped name (which would hopefulyl be 
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
    CreateQuery(db, "type data.%s", c_node); 
    std::string node_type = db.yieldResponse <std::string>();
    if (node_type == "string")
    {
        //We need no further information!
        CreateQuery(db, "get data.%s", c_node);
    }
    else if (node_type == "hash")
    {
        // If this is the case, the very next argument must inherently be
        // the hash key to resolve the field. There are two possibilities:
        MoguSyntax hashkey = __tm.currentToken <MoguSyntax> (); 

        // The key is a string, such as "foo", with syntax like: 
        //  hget data.bar foo 
        if (hashkey == MoguSyntax::TOKEN_DELIM)
            CreateQuery(db, "hget data.%s %s", c_node,
                    __tm.fetchStringToken().c_str());

        // Or the key is a string that is really an integer, with 
        // syntax like:
        //
        // hget data.bar 17
        else
            CreateQuery(db, "hget data.%s %d", c_node,
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
        CreateQuery(db, "lindex data.%s %d", c_node, __tm.currentToken<int>());

    }

    result.setString(db.yieldResponse<std::string>());
}

void StateParser::handleWidget(Moldable* widget, NodeValue& result)
{
    widget->getAttribute(__tm.currentToken <MoguSyntax>(), result);
}

void StateParser::handleUserField(const std::string& field, NodeValue& result)
{
    mApp;
    std::string userid = app->getUser();
    Redis::ContextQuery policies(Prefix::policies);
    Redis::ContextQuery user(Prefix::user);

    // Determine the expected type and encryption
    // of the field in question:
    CreateQuery(policies,
        "hget policies.%s %d", field.c_str(), MoguSyntax::encrypted);
    CreateQuery(policies,
        "hget policies.%s %d", field.c_str(), MoguSyntax::type);
    bool encrypted = policies.yieldResponse <bool>();
    MoguSyntax type = policies.yieldResponse <MoguSyntax>();
    
    switch(type)
    {
        case MoguSyntax::string:
            CreateQuery(user, "get user.%s.%s", field.c_str(), userid.c_str());
            break;
        case MoguSyntax::list:{
            int index = __tm.currentToken <int> ();
            CreateQuery(user, "lindex user.%s.%s %d",
                    field.c_str(), userid.c_str(), index);
            break;}
        case MoguSyntax::hash:
            // Make sure that the current token is a TOKEN_DELIM:
            if (__tm.currentToken <MoguSyntax>() == MoguSyntax::TOKEN_DELIM)
            {
                std::string key = __tm.fetchStringToken();
                CreateQuery(user, "hget user.%s.%s %s",
                        field.c_str(), userid.c_str(), key.c_str());
                break;
            }
            else return;
        default: return;
    };
    if (encrypted)
        result.setString(
                Security::decrypt( user.yieldResponse <std::string>()));
    else
        result.setString(user.yieldResponse <std::string>());
}

void StateParser::handleGroupField(const std::string& field, NodeValue& result)
{
    mApp;
    std::string user = app->getUser();
    int group = app->getGroup();
    GroupManager group_mgr(group);
    bool read_enabled = group_mgr.hasReadAccess(field);
    if (!read_enabled) return;
    
    Redis::ContextQuery& grpdb = group_mgr.getContext(Prefix::group); 
    Redis::ContextQuery& plcdb = group_mgr.getContext(Prefix::policies);

    CreateQuery(plcdb, "hget policies.%s %d", field.c_str(), MoguSyntax::type);
    CreateQuery(plcdb, "hget policies.%s %d",
            field.c_str(), MoguSyntax::encrypted);
    MoguSyntax type = plcdb.yieldResponse <MoguSyntax>();
    bool encrypted = plcdb.yieldResponse <bool>();

    switch(type)
    {
        case MoguSyntax::string:
            CreateQuery(grpdb, "get groups.%s.%s", group,
                    field.c_str());
            break;
        case MoguSyntax::list:{
            // Get the index of list item
            int index = __tm.currentToken <int>();
            CreateQuery(grpdb, "lindex groups.%s.%s %d", group,
                    field.c_str(), index);
            break;}
        case MoguSyntax::hash:{
            // Ensure that the next token is in fact a key:
            if (__tm.currentToken<MoguSyntax>() != MoguSyntax::TOKEN_DELIM) return;
            std::string key = __tm.fetchStringToken();
            CreateQuery(grpdb, "hget groups.%s.%s %s",
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
