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
#include <Parsers/NodeValueParser/TokenManager.h>

namespace Parsers {

class StateParser
{
	public:
		StateParser(TokenManager& tm);
		void processInput(Moldable* broadcaster);

	private:
		TokenManager& __tm;

        // When a TOKEN_DELIM is expected as a placeholder for an
        // object's identifier, this method will advance the TM pointer,
        // assure that the next token is a TOKEN_DELIM, advance the TM pointer
        // to one step after the TOKEN_DELIM, and return the resolved identifier.
        // Used as a function since it's a common operation.
        std::string getIdentifier();

        // Looks up the state of a named widget
        void handleWidget(const std::string& identifier, NodeValue& result); 
        // Looks up the state of a self-referential widget "own|self"
        void handleWidget (Moldable* broadcaster, NodeValue& result);
        void handleData(const std::string& identifier, NodeValue& result);

};

}	// namespace Parsers
#endif /* STATEPARSER_H_ */
