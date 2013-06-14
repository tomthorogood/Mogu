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
#include <Parsers/NodeValueParser/TokenManager.h>
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
		void giveInput(std::string input, NodeValue& nv,
		    Moldable* bc = nullptr);
		void giveInput(std::string input, CommandValue& cv,
		    Moldable *bc = nullptr);

	private:
		StateParser __stateParser;
		MathParser __mathParser;
		TokenManager __tm;

		void tokenizeInput(std::string input, bool setAtBeginning=false);
		bool reduceExpressions(Moldable* bc);
		int find_full_quote(std::string str);
};

}	// namespace Parsers

#endif /* NODEVALUEPARSER_H_ */
