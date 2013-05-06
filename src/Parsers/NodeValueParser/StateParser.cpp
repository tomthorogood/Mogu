/*
 * StateParser.cpp
 *
 *  Created on: March 31th, 2013
 *      Author: cameron
 */

#include <Parsers/NodeValueParser/StateParser.h>
#include <Types/syntax.h>

namespace Parsers {


StateParser::StateParser() : __objectTokens({1,2,3,4,31,40,27,62})
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
	for(auto it=rit.base()--; it!=__numTokens->end(); it++)
	{
		int currToken = *it;
		if(__objectTokens.count(currToken) == 1)
		{
			switch((MoguSyntax) currToken)
			{
				case MoguSyntax::own:
				case MoguSyntax::parent:
				case MoguSyntax::children:
				case MoguSyntax::siblings:
					break;
				case MoguSyntax::group:
				case MoguSyntax::data:
				case MoguSyntax::user:
					break;
				case MoguSyntax::widget:
					break;
				default:
					//unrecognized token
					break;
			}

		}
	}

	//debug
	if(broadcaster)
		broadcaster = 0;

}

}	// namespace Parsers
