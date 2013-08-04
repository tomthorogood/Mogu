/*
 * StateParser.cpp
 *
 *  Created on: March 31th, 2013
 *      Author: cameron
 */

#include <Mogu.h>
#include <Parsers/NodeValueParser/StateParser.h>
#include <Types/syntax.h>
#include <Redis/MoguQueryHandler.h>
#include <Security/Encryption.h>
#include <Groups/GroupManager.h>

namespace Parsers {


StateParser::StateParser(TokenManager& tm) : tm(tm)
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
    tm.next();
    if (tm.currentToken  () != MoguSyntax::TOKEN_DELIM)
    {
        //TODO throw a pretty big error, as this should only happen in the 
        //case of bad syntax checking on import, or database corruption.
    }
    std::string identifier = tm.fetchStringToken();
    tm.next();
    return identifier;
}

void StateParser::processInput(Moldable* broadcaster)
{
    tm.saveLocation();
    int currentToken = tm.currentToken ();
    NodeValue result;
    std::string identifier;

	switch(currentToken)
	{
		case MoguSyntax::widget:
            identifier = getIdentifier();
            handleWidget(identifier,result);
            break;
        case MoguSyntax::own:
            tm.next();
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

    tm.deleteFromSaved();

    switch(result.getType())
    {
        case ReadType::string_value:
            tm.injectToken(result.getString());
            break;
        case ReadType::int_value:
            tm.injectToken(result.getInt());
            break;
        default:
            // We'll figure out floats or bad values later (TODO)
            tm.injectToken(0); 
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
    int widget_attribute = tm.currentToken ();
    widget->getAttribute(MoguSyntax::get(widget_attribute), result);
}

void StateParser::handleData(const std::string& identifier, NodeValue& result)
{
    const char* c_node = identifier.c_str();
    Redis::MoguQueryHandler db(Application::contextMap, Prefix::data);
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
        int hashkey = tm.currentToken  ();

        // The key is a string, such as "foo", with syntax like: 
        //  hget data.bar foo 
        if (hashkey == MoguSyntax::TOKEN_DELIM)
            db.appendQuery( "hget data.%s %s", c_node,
                    tm.fetchStringToken().c_str());

        // Or the key is a string that is really an integer, with 
        // syntax like:
        //
        // hget data.bar 17
        else
            db.appendQuery( "hget data.%s %d", c_node,
                    (int) tm.currentToken());

        // NOTE that there is the possibility of a collision in the highly
        // unlikely case that the hash key integer value is the same as the 
        // TOKEN_DELIM integer. Again, though, this is super unlikely, but it
        // should be noted as a later TODO.
    }
    else if (node_type == "list")
    {
        // In this case, there must necessarily be a list index in order
        // to find the value.
        db.appendQuery( "lindex data.%s %d", c_node, (int) tm.currentToken());

    }

    result.setString(db.yieldResponse<std::string>());
}

void StateParser::handleWidget(Moldable* widget, NodeValue& result)
{
    if (widget == nullptr) return;
    widget->getAttribute(MoguSyntax::get(tm.currentToken ()), result);
}

void StateParser::handleUserField(const std::string& field, NodeValue& result)
{
    mApp;
    int token;
    NodeValue arg;
    Redis::NodeEditor node(Prefix::user, field);
    node.setId(app->getUser());
    int type = node.getType().integer;
    if (type == MoguSyntax::__NONE__)
    {
        type = node.policyType().integer;
    }
    if (type == MoguSyntax::hash)
    {
        token = tm.currentToken();
        if (token == MoguSyntax::TOKEN_DELIM)
            arg.setString(tm.fetchStringToken());
        else
            arg.setString(std::to_string(tm.currentToken()));
    }
    else if (type == MoguSyntax::list)
    {
        arg.setString(std::to_string(tm.currentToken()));
    }
    node.setArg(&arg);
    std::string val = node.read();

    result.setString(val);
}

void StateParser::handleGroupField(const std::string& field, NodeValue& result)
{
    mApp;
    int group = app->getGroup();
    GroupManager group_mgr(group);
    bool read_enabled = group_mgr.hasReadAccess(field);
    if (!read_enabled) return;
    
    Redis::MoguQueryHandler& grpdb = group_mgr.getContext(Prefix::group); 
    Redis::MoguQueryHandler& plcdb = group_mgr.getContext(Prefix::policies);

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
            int index = tm.currentToken();
            grpdb.appendQuery( "lindex groups.%s.%s %d", group,
                    field.c_str(), index);
            break;}
        case MoguSyntax::hash:{
            // Ensure that the next token is in fact a key:
            if (tm.currentToken() != MoguSyntax::TOKEN_DELIM) return;
            std::string key = tm.fetchStringToken();
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
