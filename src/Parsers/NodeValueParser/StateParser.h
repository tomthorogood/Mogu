/*
 * StateParser.h
 *
 *  Created on: March 31, 2013
 *      Author: cameron
 */

#ifndef STATEPARSER_H_
#define STATEPARSER_H_

#include <list>
#include <string>
#include <vector>
#include <unordered_set>

#include <Types/NodeValue.h>
#include <Moldable/Moldable.h>

namespace Parsers {

class StateParser
{
	public:
		StateParser();
		void processInput(std::list<int>::reverse_iterator& rit,
						  Moldable* broadcaster);
		void setTokens(std::list<int>& numTokens,
					   std::vector<std::string>& strTokens);

	private:
		std::list<int>* __numTokens;
		std::vector<std::string>* __strTokens;

		const std::unordered_set<int> __widgetTokens = {1,2,3,4,37,65};
		const std::unordered_set<int> __dbTokens = {28,29,33,43};
};

}	// namespace Parsers
#endif /* STATEPARSER_H_ */
