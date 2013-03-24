/*
 * AttributeParser.cpp
 *
 *  Created on: March 4th, 2013
 *      Author: cameron
 */

#include <Parsers/AttributeParser.h>
#include <iostream>

namespace Parsers {

AttributeParser::AttributeParser()
{
	__tokenizer.add_pair('"', '"');
}

void AttributeParser::giveInput(std::string input, NodeValue& v)
{
	__tokenizer.reset().newString(input);

	while(__tokenizer.hasNextToken()) {
		std::string next_token = __tokenizer.next();
		__tokens.push_back(next_token);
	}

	std::cout << "tokens are as follows:" << std::endl;
	for(int i=0; i<__tokens.size(); i++)
		std::cout << __tokens[i] << std::endl;
}



}	// namespace Parsers
