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

void StateParser::setTokens(std::list<int>& numTokens,
							std::vector<std::string>& strTokens)
{
	__numTokens = &numTokens;
	__strTokens = &strTokens;
}

void StateParser::processInput(std::list<int>::reverse_iterator &rit,
							   Moldable* broadcaster)
{
	auto it = rit.base()--;
	MoguSyntax currToken = (MoguSyntax) *it;

	if(__widgetTokens.count(currToken) == 1) {
		Moldable* target;
		switch(currToken) {
		case MoguSyntax::own:
			target = broadcaster;
			break;
		case MoguSyntax::parent:
			target = broadcaster.parent();
		case MoguSyntax::child:
		case MoguSyntax::children:
		case MoguSyntax::siblings:
		case MoguSyntax::widget:
		default:
		}
	}

	else if(__dbTokens.count(currToken) == 1) {
		switch(currToken) {
		case MoguSyntax::group:
		case MoguSyntax::user:
		case MoguSyntax::data:
		case MoguSyntax::session:
		default:
		}
	}

	else {
		switch(currToken) {
			case MoguSyntax::slot:
			default:
	}



}

}	// namespace Parsers
