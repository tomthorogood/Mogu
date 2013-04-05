/*
 * NodeValueParser.cpp
 *
 *  Created on: March 31, 2013
 *      Author: cameron
 */
#include <Parsers/NodeValueParser.h>

#include <Types/NodeValue.h>

#include <cctype>

//debug
#include <iostream>

namespace Parsers {
/* things we need to do:
 * 1.  break the string into tokens
 * 2.  detect whether we have states we need to resolve in StateParser
 * 3.  detect whether we have math we need to resolve in MathParser
 * 4.  detect whether we need to use AttributeParser or CommandParser
 */

NodeValueParser::NodeValueParser()
{
	__tokens = std::vector<NodeValue>();
}

void NodeValueParser::tokenizeInput(std::string input)
{
	//debug
	std::cout << "input string: " << input << std::endl;
	
	int inputIndex = 0;
	NodeValue* nvToken;
	while(inputIndex < input.size())
	{
		// ASSUMPTION: input is non-empty
		char delineator;
		bool isStringLiteral = false;

		if(input[inputIndex] == '"') {
			delineator = '"';
			isStringLiteral = true;
		}
		else	
			delineator = ' ';

		auto endTokenIndex = input.find(delineator, inputIndex + 1);
		auto startTokenIndex = inputIndex + (int) isStringLiteral;	//skip the "

		std::string token = input.substr(startTokenIndex, 
				endTokenIndex - startTokenIndex); 

		//this next if-else chain is going to look weird because we
		//can't test token[0] until we know that we're not dealing
		//with a string literal (possibility of empty string)
		if(isStringLiteral) {
			nvToken = new NodeValue();
			nvToken->setString(token);
			__tokens.push_back(*nvToken);
			inputIndex = endTokenIndex + 2;	//skip " and whitespace
			continue;
		}

		//from this point we're assuming that token is nonempty!
		//this means that the importer must gaurantee that no input string
		//can have two adjacent whitespaces
		else if(isdigit(token[0]))
		{
			nvToken = new NodeValue();
			nvToken->setInt(std::stoi(token));
			std::cout << "(int) " << nvToken->getInt() << std::endl;
			__tokens.push_back(*nvToken);

		}

		else
		{
			nvToken = new NodeValue();
			nvToken->setString(token);
			std::cout << nvToken->getString() << std::endl;
			__tokens.push_back(*nvToken);
		}
		
		inputIndex = endTokenIndex + 1;		//skip whitespace
	}

	//debug
	std::cout << "tokenizing finished!" << std::endl;
	
}

void NodeValueParser::giveInput(std::string input, NodeValue& v)
{
	tokenizeInput(input);
}
bool NodeValueParser::hasStates()
{
	// TODO: figure out the best way to detect states!
}

void NodeValueParser::evaluateMath()
{
}

bool NodeValueParser::detectEventCommand()
{
}

void NodeValueParser::__reset__()
{
	__tokens.clear();
}

}	// namespace Parsers
