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

#include <forward_list>
#include <string>

namespace Parsers {

/* take an input (a string from the database), tokenize it, convert
 * the values that are meant to be read as integers into integers,
 * store it all as a std::vector<NodeValue>'s (basically a vector of
 * tokens), and then run that vector through the appropriate pipeline
 * of parsers
 *
 */
class NodeValueParser 
{
	public:
		NodeValueParser();
		void giveInput(std::string input, NodeValue& v);

	private:
		StateParser __stateParser;
		MathParser __mathParser;
		AttributeParser __attParser;
		CommandParser __cmdParser;

		std::forward_list<int> __tokens;
		bool hasMath;
		bool hasStates;
		bool isEvent;

		void tokenizeInput(std::string input);
		void packString(std::string inputString);

		//debug
		void printTokens();

};

}	// namespace Parsers

#endif /* NODEVALUEPARSER_H_ */
