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

namespace Parsers {

class StateParser
{
	public:
		StateParser();
		void setParsers(std::list<int>& numTokens,
						std::vector<std::string>& strTokens);
		void processInput(std::list<int>::reverse_iterator& rit,
						  Moldable* broadcaster);

	private:
		std::list<int>* __numTokens;
		std::vector<std::string>* __strTokens;
};

}	// namespace Parsers
#endif /* STATEPARSER_H_ */
