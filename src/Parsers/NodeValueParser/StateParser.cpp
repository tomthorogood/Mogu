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

<<<<<<< HEAD
void StateParser::processInput(std::list<int>::reverse_iterator rit,
							   Moldable* broadcaster);
=======
void StateParser::setTokens(std::list<int>& numTokens,
							std::vector<std::string>& strTokens)
{
	__numTokens = numTokens;
	__strTokens = strTokens;
}

void StateParser::processInput(std::list<int>::reverse_iterator rit,
							   Moldable* broadcaster)
>>>>>>> 10507bec0743e303841951054a223078ed455f19
{

	for(auto rit=numTokens.rbegin(); rit!=numTokens.rend(); rit++)
	{
		int currToken = *rit;
		if(__objectTokens.count(currToken) == 1)
		{
			objectArgsRit = rit;

			switch(currToken)
			{
				case MoguSyntax::own:
					targetWidget = broadcaster;
				case MoguSyntax::parent:
					targetWidget = broadcaster->parent();
				case MoguSyntax::children:
					//TODO: how do we find/handle children?
				case MoguSyntax::siblings:
					//TODO: how do we find/handle siblings
					resolveRelativeWidgetInfo();
					break;
				case MoguSyntax::group:
				case MoguSyntax::data:
				case MoguSyntax::user:
					resolveDatabaseInfo();
					break;
				case MoguSyntax::m_widget:
					resolveNamedWidgetInfo();
					break;
				default:
					//TODO: handle unrecognized token!
					break;

		}
	}

}

}	// namespace Parsers
