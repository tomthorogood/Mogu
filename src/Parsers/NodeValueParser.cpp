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

NodeValueParser::NodeValueParser() : __actionTokens({16,17,18,19,21,22,23,24,25,
													 29,32,34,39,43,63})
{
}

void NodeValueParser::tokenizeInput(std::string input)
{
	int inputIndex = 0;
	while(inputIndex < input.size())
	{
		int endTokenIndex; 
		if(input[inputIndex] == '"')
			endTokenIndex = input.find('"', inputIndex);
		else
			endTokenIndex = input.find(' ', inputIndex) - 1;

		std::string token = input.substr(inputIndex, endTokenIndex-inputIndex+1);

		if(isdigit(token[0]))
			__numTokens.push_back(token);
		else
		{
			__numTokens.push_back(PACKING_DELIMITER);
			__strTokens.push_back(token);
		}

		inputIndex += 2;
	}
}

void NodeValueParser::printTokens()
{
	std::cout << "Contents of __numTokens:" << std::endl;
	for(auto it = __numTokens.begin(); it != __numTokens.end(); it++)
		std::cout << *it << std::endl;
	std::cout << "End __numTokens list." << std::endl << std::endl;

	std::cout << "Contents of __strTokens:" << std::endl;
	for(auto it = __strTokens.begin(); it != __strTokens.end(); it++)
		std::cout << *it << std::endl;
	std::cout << "End __strTokens list." << std::endl << std::endl;
}

void NodeValueParser::giveInput(std::string input, NodeValue& v)
{
	tokenizeInput(input);
	printTokens();

	//single-token input
	if(__numTokens.size() == 1)
	{
		if(__numTokens[0] == PACKING_DELIMITER)
			v.setString(__strTokens[0]);
		else
			v.setInt(__numTokens[0]);

		return;
	}
	__stateParser.processInput(__numTokens, __strTokens);
	__mathParser.processInput(__numTokens, __strTokens);

	//if the first token is an attribute enum, send through the
	//command parser; otherwise, send through attribute parser 
	if(__actionTokens.count(__numTokens[0]) == 1)
		//TODO: how is command parser going to return event data?
		__cmdParser.processInput(__numTokens, __strTokens, v);
	else
		__attParser.processInput(__numTokens, __strTokens, v);
}


}	// namespace Parsers
