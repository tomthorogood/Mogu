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

	if(isWidgetToken(currToken)) {

		Moldable* target;
		switch(currToken) {
			case MoguSyntax::own:
				target = broadcaster;
				break;
			case MoguSyntax::parent:
				target = broadcaster.parent();
				break;
			case MoguSyntax::child:
				target = broadcaster.children()[0];
				break;
			case MoguSyntax::widget:
				it++;	//place iterator on identifier
				if
				target = app->registeredWidget(*it);
				break;
		}

	}

	else if(isDBToken(currToken)) {
		int index = *(it++);
		std::string identifier = __strTokens->at(index);
		int arg = *(it++);



		switch(currToken) {
			case MoguSyntax::user:
				Redis::ContextQuery db(Prefix::user);
				break;
			case MoguSyntax::group:
				Redis::ContextQuery db(Prefix::group);
				break;
			case MoguSyntax::data:
				Redis::ContextQuery db(Prefix::data);
				break;
		}
	}

	else 		//slot syntax?
	{
	}


}


}	// namespace Parsers
