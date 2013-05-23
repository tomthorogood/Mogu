/*
 * StateParser.h
 *
 *  Created on: March 31, 2013
 *      Author: cameron
 */

#ifndef STATEPARSER_H_
#define STATEPARSER_H_

#include <string>

#include <declarations.h>
#include <Moldable/Moldable.h>
#include <Parsers/NodeValueParser/TokenGroups.h>
#include <Parsers/NodeValueParser/TokenManager.h>

namespace Parsers {

class StateParser
{
	public:
		StateParser();
		void processInput(Moldable* broadcaster);
		void setTokenManager(TokenManager& tm);

	private:
		TokenManager& __tm;


};

}	// namespace Parsers
#endif /* STATEPARSER_H_ */
