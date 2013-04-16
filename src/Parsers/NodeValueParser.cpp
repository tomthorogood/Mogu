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
#define PACKING_DELIMITER 999999

namespace Parsers {

typedef std::forward_list<int>::iterator ListIterator;

NodeValueParser::NodeValueParser()
{
}

void NodeValueParser::tokenizeInput(std::string input)
{
	int inputIndex = 0;
	ListIterator tokensIndex = __tokens.before_begin();
	while(inputIndex < input.size())
	{
		int endTokenIndex; 
		if(input[inputIndex] == '"')
			endTokenIndex = input.find('"', inputIndex);
		else
			endTokenIndex = input.find(' ', inputIndex) - 1;

		std::string token = input.substr(inputIndex, endTokenIndex-inputIndex+1);

		if(isdigit(token[0]))
			tokensIndex = __tokens.insert_after(tokensIndex, std::stoi(token));
		else
			packString(token, tokensIndex);

		inputIndex += 2;
	}
}

void NodeValueParser::packString(std::string inputString, ListIterator &tokensIndex)
{
	unsigned int packedInt = 0;
	int packedIntIndex = 0;
	int inputIndex = 0;

	while(inputIndex < inputString.size())
	{
		packedInt |= inputString[inputIndex] << (8 * packedIntIndex);
		inputIndex++;

		if(packedIntIndex == 3)
		{
			tokensIndex = __tokens.insert_after(tokensIndex, packedInt);
			packedIntIndex = 0;
			packedInt = 0;
			continue;
		}

		packedIntIndex++;
	}

	if(packedIntIndex != 0)
		tokensIndex = __tokens.insert_after(tokensIndex, packedInt);

	inputIndex++;
}

void NodeValueParser::printTokens()
{
	std::cout << "Contents of __tokens:" << std::endl;
	for(auto it = __tokens.begin(); it != __tokens.end(); it++)
		std::cout << *it << std::endl;
	std::cout << "End __tokens list." << std::endl;
}

void NodeValueParser::giveInput(std::string input, NodeValue& v)
{
	tokenizeInput(input);
	printTokens();
}

void NodeValueParser::giveInput(const char* input, NodeValue& v)
{
	std::cout << "input string: " << input << std::endl << std::endl;
	tokenizeInput(input);


	std::cout << "tokens:";
	for(auto it = __tokens.begin(); it != __tokens.end(); ++it)
		std::cout << ' ' << *it;
	std::cout << std::endl;
}


}	// namespace Parsers
