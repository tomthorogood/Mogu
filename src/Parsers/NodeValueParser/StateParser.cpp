/*
 * StateParser.cpp
 *
 *  Created on: March 31th, 2013
 *      Author: cameron
 */

#include <Parsers/NodeValueParser/StateParser.h>
#include <Types/syntax.h>

namespace Parsers {


StateParser::StateParser()
{
}

void StateParser::setTokens(std::vector<int>& numTokens,
							std::vector<std::string>& strTokens)
{
	__numTokens = &numTokens;
	__strTokens = &strTokens;
}

void StateParser::processInput(std::vector<int>::reverse_iterator &rit,
							   Moldable* broadcaster)
{
	mApp;
	auto it = rit.base()--;
	MoguSyntax currToken = (MoguSyntax) *it;

	Moldable* target;
	switch(currToken) {
		case MoguSyntax::own:
			target = broadcaster;
		case MoguSyntax::parent:
			target = (Moldable*) broadcaster->parent();
		case MoguSyntax::child:
			//assumes we want only the first child.  how do we address
			//multiple children?
			target = (Moldable*) (broadcaster->children()[0]);
		case MoguSyntax::widget:
			target = app->registeredWidget( );
			break;

		case MoguSyntax::user:
		case MoguSyntax::session:
		case MoguSyntax::group:
		case MoguSyntax::data:
			break;

		case MoguSyntax::slot:
			break;

		default:
			break;
	}

}


}	// namespace Parsers
