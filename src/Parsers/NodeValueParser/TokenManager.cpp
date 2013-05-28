/*
 * TokenManager.cpp
 *
 *  Created on: May 7th, 2013
 *      Author: cameron
 */

#include <Parsers/NodeValueParser/TokenManager.h>

//debug
#include <iostream>

namespace Parsers {

TokenManager::TokenManager()
{
}

void TokenManager::reset()
{
	__numTokens.clear();
	__strTokens.clear();
}

void TokenManager::addToken(int numToken)
{
	__numTokens.push_back(numToken);
}

void TokenManager::addToken(std::string strToken)
{
	__numTokens.push_back((int) MoguSyntax::TOKEN_DELIM);
	__strTokens.push_back(strToken);
}

//must call this function directly after all tokens are added
//or else we will have undefined behavior!
void TokenManager::setIterator()
{
	__it = --(__numTokens.end());
	__strIndex = __strTokens.size();	//initially points out-of-bounds

	if(*__it == (int) MoguSyntax::TOKEN_DELIM)
		__strIndex--;	//in case we don't have a prev() call to properly 
						//line us up
}

std::string TokenManager::fetchStringToken()
{
	if(*__it == (int) MoguSyntax::TOKEN_DELIM)
		return __strTokens[__strIndex];

	else
		return R"(ERR: DEREFERENCING NON-TOKENDELIM)";
}

//TODO: we are dereferencing our iterator twice; once on iterator
//incr/decr, and again when/if we fetchStringToken.  refactor

void TokenManager::next()
{
	__it++;
	if(*__it == (int) MoguSyntax::TOKEN_DELIM)
	   __strIndex++;	

	if(__delStringCount >= 0)
		__delStringCount++;
}

void TokenManager::prev()
{
	__it--;
	if(*__it == (int) MoguSyntax::TOKEN_DELIM)
		__strIndex--;

	if(__delStringCount >= 0)
		__delStringCount--;
}

void TokenManager::saveLocation()
{
	__savedit = __it;

	__delStringCount = 0;
	if(*__it == (int) MoguSyntax::TOKEN_DELIM)
		__delStringCount++;
}

void TokenManager::deleteToSaved()
{
	__it = __numTokens.erase(__it, __savedit+1);
	__strIndex -= __delStringCount;
	if(*__it != (int) MoguSyntax::TOKEN_DELIM)
		__strIndex++;
	__delStringCount = -1;
}

void TokenManager::deleteFromSaved()
{
	__it = __numTokens.erase(__savedit, __it+1);
	__strIndex -= __delStringCount;
	if(*__it != (int) MoguSyntax::TOKEN_DELIM)
		__strIndex++;
	__delStringCount = -1;
}

//call this directly after deleteToSaved()!
void TokenManager::injectToken(int numToken)
{
	if(*__it == (int) MoguSyntax::TOKEN_DELIM)
		__strIndex++;

	__it = __numTokens.insert(__it, numToken);
}

//call this directly after deleteToSaved()!
void TokenManager::injectToken(std::string strToken)
{
	__it = __numTokens.insert(__it, (int) MoguSyntax::TOKEN_DELIM);
	__strTokens.insert(__strTokens.begin()+__strIndex+1, strToken);
	__strIndex++;
}

//debug method
void TokenManager::printTokens()
{
	int tempStrIndex = 0;
	std::cout << "[TokenManager]: ";
	for(unsigned int i=0; i<__numTokens.size(); i++)
	{
		if(__numTokens[i] == (int) MoguSyntax::TOKEN_DELIM)
			std::cout << __strTokens[tempStrIndex++] << " ";
		else
			std::cout << __numTokens[i] << " ";
	}
	std::cout << std::endl;
}

template <typename T> T TokenManager::currentToken() {
    return (T) currentToken <int>();
}

template <> int TokenManager::currentToken()
{
	if(__it < __numTokens.begin())
		return (int) OutOfRange::Begin;

	else if(__it >= __numTokens.end())
		return (int) OutOfRange::End;

	else
		return *__it;
}

template <> MoguSyntax TokenManager::currentToken()
{
    return static_cast<MoguSyntax>(currentToken <int>());
}


}	// namespace Parsers
