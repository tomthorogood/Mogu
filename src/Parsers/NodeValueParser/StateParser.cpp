/*
 * State_Parser.cpp
 *
 *  Created on: March 31th, 2013
 *      Author: cameron
 */

#include <Mogu.h>
#include <Parsers/Node_ValueParser/State_Parser.h>
#include <Types/syntax.h>
#include <Redis/MoguQueryHandler.h>
#include <Security/Encryption.h>
#include <Groups/GroupManager.h>


namespace Parsers {

/* In most circumstances, we will require an identifier to go along 
 * with an object. When required, this function will advance the 
 * TokenManager's pointer, assert that an Identifier was found, retrieve
 * the identifier (and return it), and advance the TokenManager to the next
 * pointer.
 */
std::string State_Parser::get_identifier()
{
    tm.next();
    if (tm.current_token  () != Mogu_Syntax::TOKEN_DELIM)
    {
        //TODO throw a pretty big error, as this should only happen in the 
        //case of bad syntax checking on import, or database corruption.
    }
    std::string s {tm.fetch_string()};
    tm.next();
    return s;
}

void State_Parser::process_input(Moldable* broadcaster)
{
    tm.save_location();

    int curr_token {tm.current_token()};
    Node_Value result {}
    std::string identifier {}

	switch(curr_token)
	{
		case Mogu_Syntax::widget:
            identifier = get_identifier();
            handle_widget(identifier,result);
            break;
        case Mogu_Syntax::own:
            tm.next();
            handle_widget(broadcaster, result);
            break;
		case Mogu_Syntax::data:
            identifier = get_identifier();
            handle_data(identifier, result);
            break;
		case Mogu_Syntax::user:
            identifier = get_identifier();
            handle_user_field(identifier, result);
            break;
		case Mogu_Syntax::group:
            identifier = get_identifier();
            handle_group_field(identifier, result);
            break;
		case Mogu_Syntax::slot:{
            mApp;
            identifier = get_identifier();
            result = app->slotManager().retrieveSlot(identifier);
            break;
          }
        default:{
            result.set_int((int)current_token);
            break;
        }
	}

    tm.deleteFromSaved();

    switch(result.getType())
    {
        case ReadType::string_value:
            tm.injectToken(result.get_string());
            break;
        case ReadType::int_value:
            tm.injectToken(result.get_int());
            break;
        default:
            // We'll figure out floats or bad values later (TODO)
            tm.injectToken(0); 
            break;
    }

}

// Handles the resolution of a named widget's states
void State_Parser::handle_widget(const std::string& identifier, Node_Value& result) 
{
    mApp;
    Moldable* widget = app->registeredWidget(identifier);

    // For now, just ignore everything if the widget is not actually registered
    // due to premature deletion, or a mistyped name (which would hopefully be
    // caught on import).
    // TODO
    if (widget == nullptr) return; 
    
    // get_identifier will have already advaned the token pointer 
    int widget_attribute = tm.current_token ();
    widget->get_attribute(Mogu_Syntax::get(widget_attribute), result);
}

void State_Parser::handle_data(const std::string& identifier, Node_Value& result)
{
    const char* c_node = identifier.c_str();
    Redis::MoguQueryHandler db(Application::contextMap, Prefix::data);
    db.append_query( "type data.%s", c_node); 
    std::string node_type = db.yieldResponse <std::string>();
    if (node_type == "string")
    {
        //We need no further information!
        db.append_query( "get data.%s", c_node);
    }
    else if (node_type == "hash")
    {
        // If this is the case, the very next argument must inherently be
        // the hash key to resolve the field. There are two possibilities:
        int hashkey = tm.current_token  ();

        // The key is a string, such as "foo", with syntax like: 
        //  hget data.bar foo 
        if (hashkey == Mogu_Syntax::TOKEN_DELIM)
            db.append_query( "hget data.%s %s", c_node,
                    tm.fetch_string().c_str());

        // Or the key is a string that is really an integer, with 
        // syntax like:
        //
        // hget data.bar 17
        else
            db.append_query( "hget data.%s %d", c_node,
                    (int) tm.current_token());

        // NOTE that there is the possibility of a collision in the highly
        // unlikely case that the hash key integer value is the same as the 
        // TOKEN_DELIM integer. Again, though, this is super unlikely, but it
        // should be noted as a later TODO.
    }
    else if (node_type == "list")
    {
        // In this case, there must necessarily be a list index in order
        // to find the value.
        db.append_query( "lindex data.%s %d", c_node, (int) tm.current_token());

    }

    result.set_string(db.yieldResponse<std::string>());
}

void State_Parser::handle_widget(Moldable* widget, Node_Value& result)
{
    if (widget == nullptr) return;
    widget->get_attribute(Mogu_Syntax::get(tm.current_token ()), result);
}

void State_Parser::handle_user_field(const std::string& field, Node_Value& result)
{
    mApp;
    int token;
    Node_Value arg;
    Redis::Node_Editor node(Prefix::user, field);
    node.setId(app->getUser());
    int type = node.getType().integer;
    if (type == Mogu_Syntax::__NONE__)
    {
        type = node.policyType().integer;
    }
    if (type == Mogu_Syntax::hash)
    {
        token = tm.current_token();
        if (token == Mogu_Syntax::TOKEN_DELIM)
            arg.set_string(tm.fetch_string());
        else
            arg.set_string(std::to_string(tm.current_token()));
    }
    else if (type == Mogu_Syntax::list)
    {
        arg.set_string(std::to_string(tm.current_token()));
    }
    node.setArg(&arg);
    std::string val = node.read();

    result.set_string(val);
}

void State_Parser::handle_group_field(const std::string& field, Node_Value& result)
{
    mApp;
    int group = app->getGroup();
    GroupManager group_mgr(group);
    bool read_enabled = group_mgr.hasReadAccess(field);
    if (!read_enabled) return;
    
    Redis::MoguQueryHandler& grpdb = group_mgr.getContext(Prefix::group); 
    Redis::MoguQueryHandler& plcdb = group_mgr.getContext(Prefix::policies);

    plcdb.append_query( "hget policies.%s %d", field.c_str(), (int)Mogu_Syntax::type);
    plcdb.append_query( "hget policies.%s %d",
            field.c_str(), (int) Mogu_Syntax::encrypted);
    const SyntaxDef& type = Mogu_Syntax::get(plcdb.yieldResponse<std::string>());
    bool encrypted = plcdb.yieldResponse <bool>();

    switch(type)
    {
        case Mogu_Syntax::string:
            grpdb.append_query( "get groups.%s.%s", group,
                    field.c_str());
            break;
        case Mogu_Syntax::list:{
            // Get the index of list item
            int index = tm.current_token();
            grpdb.append_query( "lindex groups.%s.%s %d", group,
                    field.c_str(), index);
            break;}
        case Mogu_Syntax::hash:{
            // Ensure that the next token is in fact a key:
            if (tm.current_token() != Mogu_Syntax::TOKEN_DELIM) return;
            std::string key = tm.fetch_string();
            grpdb.append_query( "hget groups.%s.%s %s",
                    group, field.c_str(), key.c_str());
            break;}
        default: return;
    };

    if (encrypted)
        result.set_string(
            Security::decrypt(
                grpdb.yieldResponse <std::string>()));
    else
        result.set_string(grpdb.yieldResponse <std::string>());
}
}	// namespace Parsers
