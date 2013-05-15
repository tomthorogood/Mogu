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

#include <declarations.h>
#include <Types/NodeValue.h>
#include <Moldable/Moldable.h>
#include <Parsers/NodeValueParser/TokenGroups.h>

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
		std::vector<std::string>* __strTokens;


};

}	// namespace Parsers
#endif /* STATEPARSER_H_ */
