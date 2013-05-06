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
class NodeValue;
class CommandValue;

#include <list>
#include <string>
#include <vector>

namespace Parsers {

inline int mtoi(MoguSyntax input) {
	return static_cast<int>(input);
}

class NodeValueParser 
{
	public:
		NodeValueParser();
		void giveInput(std::string input, NodeValue& nv, Moldable* bc = 0);
        //void giveInput(std::string input, NodeValue& nv);
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
		void parseListener(std::list<int>::iterator& it, CommandValue& cv);
		void parseMessage(std::list<int>::iterator& it, CommandValue& cv);

		//debug
		void printTokens();

};

}	// namespace Parsers

#endif /* NODEVALUEPARSER_H_ */
