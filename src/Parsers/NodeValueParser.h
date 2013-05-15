/*
 * NodeValueParser.h
 *
 *  Created on: March 31, 2013
 *      Author: cameron
 */

#ifndef NODEVALUEPARSER_H_
#define NODEVALUEPARSER_H_

#include <Parsers/NodeValueParser/StateParser.h>
#include <Parsers/NodeValueParser/MathParser.h>
#include <Parsers/NodeValueParser/TokenGroups.h>
#include <Types/syntax.h>
class NodeValue;
class CommandValue;

#include <string>
#include <vector>

namespace Parsers {

//TODO: clear all the token vectors for new iteration of NVP

class NodeValueParser 
{
	public:
		NodeValueParser();
		void giveInput(std::string input, NodeValue& nv, Moldable* bc = 0);
		void giveInput(std::string input, CommandValue& cv);

	private:
		StateParser __stateParser;
		MathParser __mathParser;

		std::vector<int> __numTokens;
		std::vector<std::string> __strTokens;

		

		void tokenizeInput(std::string input);
		bool reduceExpressions(Moldable* bc);
		void parseListener(std::vector<int>::iterator& it, CommandValue& cv);
		void parseMessage(std::vector<int>::iterator& it, CommandValue& cv);

		//debug
		void printTokens();

};

}	// namespace Parsers

#endif /* NODEVALUEPARSER_H_ */
