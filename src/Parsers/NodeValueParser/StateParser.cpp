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
    if (__tm.currentToken  () != MoguSyntax::TOKEN_DELIM)
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
    SyntaxDef currentToken = __tm.currentToken ();
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
    const SyntaxDef& widget_attribute = __tm.currentToken ();
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
        const SyntaxDef& hashkey = __tm.currentToken  (); 

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
                    (int) __tm.currentToken());

        // NOTE that there is the possibility of a collision in the highly
        // unlikely case that the hash key integer value is the same as the 
        // TOKEN_DELIM integer. Again, though, this is super unlikely, but it
        // should be noted as a later TODO.
    }
    else if (node_type == "list")
    {
        // In this case, there must necessarily be a list index in order
        // to find the value.
        db.appendQuery( "lindex data.%s %d", c_node, (int) __tm.currentToken());

    }

    result.setString(db.yieldResponse<std::string>());
}

void StateParser::handleWidget(Moldable* widget, NodeValue& result)
{
    if (widget == nullptr) return;
    widget->getAttribute(__tm.currentToken (), result);
}

void StateParser::handleUserField(const std::string& field, NodeValue& result)
{
    Redis::ContextQuery policies(Prefix::policies);
    Redis::ContextQuery user(Prefix::user);
    int token;
    NodeValue arg;


    Redis::NodeEditor node(Prefix::user, field);
    if (!node.exists())
    {
        token = __tm.currentToken();
        if (__tm.currentToken() == MoguSyntax::TOKEN_DELIM)
        {
            arg.setString(__tm.fetchStringToken());
        }
        else (arg.setInt(__tm.currentToken()));

        node.setPrefix(Prefix::policies);
        std::string sub = (std::string) MoguSyntax::default_;
        node.toggleSub(sub);

        node.setArg(&arg);
        if (node.subExists(sub))
            result.setString(node.readSub(sub));
        return;
    }
    const SyntaxDef& type = node.getType();
    if (type == MoguSyntax::hash)
    {
        token = __tm.currentToken();
        if (token == MoguSyntax::TOKEN_DELIM)
            arg.setString(__tm.fetchStringToken());
        else
            arg.setInt(__tm.currentToken());
    }
    else if (type == MoguSyntax::list)
    {
        arg.setInt(__tm.currentToken());
    }
    node.setArg(&arg);
    result.setString(node.read());

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

    plcdb.appendQuery( "hget policies.%s %d", field.c_str(), (int)MoguSyntax::type);
    plcdb.appendQuery( "hget policies.%s %d",
            field.c_str(), (int) MoguSyntax::encrypted);
    const SyntaxDef& type = MoguSyntax::get(plcdb.yieldResponse<std::string>());
    bool encrypted = plcdb.yieldResponse <bool>();

    switch(type)
    {
        case MoguSyntax::string:
            grpdb.appendQuery( "get groups.%s.%s", group,
                    field.c_str());
            break;
        case MoguSyntax::list:{
            // Get the index of list item
            int index = __tm.currentToken();
            grpdb.appendQuery( "lindex groups.%s.%s %d", group,
                    field.c_str(), index);
            break;}
        case MoguSyntax::hash:{
            // Ensure that the next token is in fact a key:
            if (__tm.currentToken() != MoguSyntax::TOKEN_DELIM) return;
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
