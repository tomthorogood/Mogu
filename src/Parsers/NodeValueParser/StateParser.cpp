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
            __tm.next();
            identifier = getIdentifier();
            //result changed in place to hold value
            handleWidget(identifier,result);
            break;
		case MoguSyntax::data:
		case MoguSyntax::user:
		case MoguSyntax::session:
		case MoguSyntax::group:
		case MoguSyntax::slot:
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
    
    MoguSyntax widget_attribute = __tm.currentToken <MoguSyntax>();
    widget->getAttribute(widget_attribute, result);
}

}	// namespace Parsers
