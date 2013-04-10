/*
 * StateParser.h
 *
 *  Created on: March 31, 2013
 *      Author: cameron
 */

#ifndef STATEPARSER_H_
#define STATEPARSER_H_

#include <forward_list>

namespace Parsers {

class StateParser
{
	public:
		StateParser();
		void processInput(const std::forward_list<int>& tokens);

};

}	// namespace Parsers
#endif /* STATEPARSER_H_ */
