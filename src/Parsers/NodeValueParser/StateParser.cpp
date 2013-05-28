/*
 * StateParser.cpp
 *
 *  Created on: March 31th, 2013
 *      Author: cameron
 */

#include <Parsers/NodeValueParser/StateParser.h>
#include <Redis/ContextQuery.h>
#include <Types/syntax.h>

namespace Parsers {


StateParser::StateParser()
{
}

void StateParser::setTokenManager(TokenManager& tm)
{
	__tm = &tm;
}

void StateParser::processInput(Moldable* broadcaster)
{
	mApp;
	MoguSyntax currToken = (MoguSyntax) __tm->currentToken();

	switch(currToken)
	{
		case MoguSyntax::widget:
		case MoguSyntax::data:
		case MoguSyntax::user:
		case MoguSyntax::session:
		case MoguSyntax::group:
		case MoguSyntax::slot:
	}

}


}	// namespace Parsers
