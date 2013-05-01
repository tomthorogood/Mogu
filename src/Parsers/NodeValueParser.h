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
#include <Parsers/NodeValueParser/AttributeParser.h>
#include <Parsers/NodeValueParser/CommandParser.h>
class NodeValue;
class CommandValue;

#include <list>
#include <string>
#include <vector>

namespace Parsers {

class NodeValueParser 
{
	public:
		NodeValueParser();
		void giveInput(std::string input, NodeValue& nv, Moldable* bc);
		void giveInput(std::string input, CommandValue& cv);

	private:
		StateParser __stateParser;
		MathParser __mathParser;

		std::list<int> __numTokens;
		std::vector<std::string> __strTokens;
		const std::unordered_set<int> __flippedActionTokens;
		const std::unordered_set<int> __objectTokens;
		const std::unordered_set<int> __prepositionTokens;

		void tokenizeInput(std::string input);
		bool reduceExpressions(Moldable* bc);

		//debug
		void printTokens();

};

}	// namespace Parsers

#endif /* NODEVALUEPARSER_H_ */
