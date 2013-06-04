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

NodeValueParser::NodeValueParser() : __stateParser(__tm), __mathParser(__tm)
{
}


void NodeValueParser::tokenizeInput(std::string input)
{
	unsigned int inputIndex = 0;
	while(inputIndex < input.size())
	{
		int endTokenIndex; 
		if(input[inputIndex] == '"')
			endTokenIndex = input.find('"', inputIndex);
		else
			endTokenIndex = input.find(' ', inputIndex) - 1;

		std::string token = input.substr(inputIndex, endTokenIndex-inputIndex+1);

		if(isdigit(token[0]))
			__tm.addToken(std::stoi(token));
		else
			__tm.addToken(token);

		inputIndex += 2;
	}

	//tell TokenManager that we're done adding tokens
	__tm.setIterator();
}

bool NodeValueParser::reduceExpressions(Moldable* bc)
{
	//MoguSyntax lastToken = MoguSyntax::__NONE__;
	MoguSyntax currToken = __tm.currentToken<MoguSyntax>();
	bool hasPreposition = false;

	while((int) currToken != (int) TokenManager::OutOfRange::Begin)
	{
		if(currToken == MoguSyntax::OPER_OPPAREN) 
			__mathParser.processInput();
			
		else if(isObjectToken(currToken))
		{
			//if(lastToken < MoguSyntax::OPER_OPPAREN)
				__stateParser.processInput(bc);
		}

		else if(isPrepositionToken(currToken))
			hasPreposition = true;

		//lastToken = __tm.currentToken<MoguSyntax>();
		__tm.prev();

		currToken = __tm.currentToken<MoguSyntax>();
	}

	return hasPreposition;
}

/*void NodeValueParser::parseListener(std::vector<int>::iterator& it,
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
}*/

void NodeValueParser::giveInput(std::string input, NodeValue& nv, Moldable* bc)
{
	tokenizeInput(input);
	reduceExpressions(bc);

	//if we have more than one token in __numTokens at this point (or
	//two if the first token is TOKEN DELIM), something has gone wrong

	if(__tm.currentToken<MoguSyntax>() == MoguSyntax::TOKEN_DELIM)
		nv.setString(__tm.fetchStringToken());
	else
		nv.setInt(__tm.currentToken<int>());
}

void NodeValueParser::giveInput(std::string input, CommandValue& cv)
{
	//CV PROCESSING PENDING CHANGES
	/*tokenizeInput(input);
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
	}*/
}

}	// namespace Parsers
