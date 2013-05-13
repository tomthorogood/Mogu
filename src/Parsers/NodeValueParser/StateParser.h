/*
 * StateParser.h
 *
 *  Created on: March 31, 2013
 *      Author: cameron
 */

#ifndef STATEPARSER_H_
#define STATEPARSER_H_

#include <string>
#include <vector>
#include <unordered_set>

class StringTracker;
#include <declarations.h>
#include <Types/NodeValue.h>
#include <Moldable/Moldable.h>

namespace Parsers {

class StateParser
{
	public:
		StateParser();
		void processInput(std::vector<int>::reverse_iterator& rit,
						  Moldable* broadcaster);
		void setTokens(std::vector<int>& numTokens,
					   std::vector<std::string>& strTokens);

	private:
		std::vector<int>* __numTokens;
		std::vector<int>* __strTokens;

		const std::unordered_set<int> __widgetTokens = {
			(int) MoguSyntax::own,
			(int) MoguSyntax::parent,
			(int) MoguSyntax::child,
			(int) MoguSyntax::widget
		};

		const std::unordered_set<int> __dbTokens = {
			(int) MoguSyntax::user,
			(int) MoguSyntax::session,
			(int) MoguSyntax::group,
			(int) MoguSyntax::data
		};

};

}	// namespace Parsers
#endif /* STATEPARSER_H_ */
