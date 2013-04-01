/*
 * NodeValueParser.cpp
 *
 *  Created on: March 31, 2013
 *      Author: cameron
 */

#include <Parsers/NodeValueParser/StateParser.h>
#include <Parsers/NodeValueParser/MathParser.h>
#include <Parsers/NodeValueParser/AttributeParser.h>
#include <Parsers/NodeValueParser/CommandParser.h>

namespace Parsers {
/* things we need to do:
 * 1.  break the string into tokens
 * 2.  detect whether we have states we need to resolve in StateParser
 * 3.  detect whether we have math we need to resolve in MathParser
 * 4.  detect whether we need to use AttributeParser or CommandParser
 */

NodeValueParser::NodeValueParser()
{
	__tokenizer.add_pair('"', '"');		//for string literals
	__tokenizer.add_pair('(', ')');		//for math strings
}

void NodeValueParser::giveInput(std::string input, NodeValue& v)
{
	__tokenizer.reset().newString(input);

	// convert the input tokens into NodeValues and push
	while(__tokenizer.hasNext()) {
		std::string stringToken = __tokenizer.next();
		NodeValue nvToken; 
		tempToken.setString(stringToken)
		__tokens.push_back(nvToken);
	}

	// run appropriate parsers
	if(hasStates())
		__stateParser.giveInput(__tokens);

	evaluateMath();	//decides whether to invoke mathparser on a token-by-token basis


	if(detectEventCommand())
		__cmdParser.giveInput(__tokens, v);
	else
		__attParser.giveInput(__tokens, v);
}

bool NodeValueParser::hasStates()
{
	// TODO: figure out the best way to detect states!
}

void NodeValueParser::evaluateMath()
{
	for(int i=0; i<__tokens.size(); i++) {
		if(__tokenizer.isWrapped(__tokens[i], '('))
				__mathParser.giveInput(__tokens[i]);
	}
}

bool NodeValueParser::detectEventCommand()
{
}

void NodeValueParser::__reset__()
{
	__tokenizer.reset();
	__tokens.clear();

}

}	// namespace Parsers
