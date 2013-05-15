/*
 * NodeValueParser.cpp
 *
 *  Created on: March 31, 2013
 *      Author: cameron
 */
#include <Parsers/NodeValueParser.h>
#include <Types/NodeValue.h>
#include <Types/syntax.h>
#include <Events/CommandValue.h>
#include <cctype>

//debug
#include <iostream>

namespace Parsers {

NodeValueParser::NodeValueParser()
{
	__stateParser.setTokens(__numTokens, __strTokens);
	__mathParser.setTokens(__numTokens);
}


void NodeValueParser::tokenizeInput(std::string input)
{
	unsigned int inputIndex = 0;
	unsigned int strIndex = 0;
	while(inputIndex < input.size())
	{
		int endTokenIndex; 
		if(input[inputIndex] == '"')
			endTokenIndex = input.find('"', inputIndex);
		else
			endTokenIndex = input.find(' ', inputIndex) - 1;

		std::string token = input.substr(inputIndex, endTokenIndex-inputIndex+1);

		if(isdigit(token[0]))
			__numTokens.push_back(std::stoi(token));
		else
		{
			__strTokens.push_back(token);
			__numTokens.push_back((int) MoguSyntax::TOKEN_DELIM);
			__numTokens.push_back(200 + strIndex);
			//magic number 200 to avoid collision with actual syntax

			strIndex++;
		}

		inputIndex += 2;
	}
}

bool NodeValueParser::reduceExpressions(Moldable* bc)
{
	MoguSyntax lastToken = MoguSyntax::__NONE__;
	bool hasPreposition = false;
	for(auto rit=__numTokens.rbegin(); rit!=__numTokens.rend(); rit++)
	{
		MoguSyntax currToken = (MoguSyntax) *rit;

		if(currToken == MoguSyntax::OPER_OPPAREN) 
			__mathParser.processInput(rit);
			
		else if(isObjectToken(currToken))
		{
			if(lastToken < MoguSyntax::OPER_OPPAREN)
				__stateParser.processInput(rit, bc);
		}

		else if(isPrepositionToken(currToken))
			hasPreposition = true;

		lastToken = (MoguSyntax) *rit;
	}

	return hasPreposition;
}

void NodeValueParser::parseListener(std::vector<int>::iterator& it,
									CommandValue& cv)
{
	cv.setObject(*it++);

	if(it == __numTokens.end() || isPrepositionToken((MoguSyntax) *it))
		return;
	cv.setIdentifier(*it++);

	if(it == __numTokens.end() || isPrepositionToken((MoguSyntax) *it))
		return;
	cv.setArg(*it++);
}

void NodeValueParser::parseMessage(std::vector<int>::iterator& it,
								   CommandValue& cv)
{
	//TODO: handling of edge cases
	//e.g. 'append widget foo to widget bar'
	cv.setValue(*it);
}

void NodeValueParser::printTokens()
{
	std::cout << "Contents of __numTokens:" << std::endl;
	for(auto it = __numTokens.begin(); it != __numTokens.end(); it++)
		std::cout << *it << std::endl;
	std::cout << "End __numTokens list." << std::endl << std::endl;

	std::cout << "Contents of __strTokens:" << std::endl;
	//for(auto it = __strTokens.begin(); it != __strTokens.end(); it++)
	//	std::cout << *it << std::endl;
	std::cout << "End __strTokens list." << std::endl << std::endl;
}

void NodeValueParser::giveInput(std::string input, NodeValue& nv, Moldable* bc)
{
	tokenizeInput(input);
	printTokens();
	reduceExpressions(bc);

	//if we have more than one token in __numTokens at this point (or
	//two if the first token is TOKEN DELIM), something has gone wrong

	if(__numTokens.front() == (int) MoguSyntax::TOKEN_DELIM)
		nv.setString(__strTokens[200 + __numTokens[1] ]);
	else
		nv.setInt(__numTokens.front());
}

void NodeValueParser::giveInput(std::string input, CommandValue& cv)
{
	tokenizeInput(input);
	bool hasPreposition = reduceExpressions(&cv.getWidget());
	cv.setAction(__numTokens.front());

	auto it = ++__numTokens.begin();
	if(isFlippedActionToken(cv.getAction()))
	{
		//flip-flopped command syntax
		//e.g. 'remove', 'append'
		parseMessage(it, cv);
		if(hasPreposition)
			parseListener(++it, cv);
	}
	else
	{
		//regular ordering
		parseListener(it, cv);
		if(hasPreposition)
			parseMessage(++it, cv);
	}
}

}	// namespace Parsers
