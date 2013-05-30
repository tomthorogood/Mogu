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
	__strIndex = __strTokens.size();	//starts out-of-bounds

	if(*__it == (int) MoguSyntax::TOKEN_DELIM)
		__strIndex--;

	std::cout << "setIterator(): strIndex = " << __strIndex << std::endl;
}

std::string TokenManager::fetchStringToken()
{
	//debug
	printStringTokens();
	std::cout << "fetchStringToken(): strIndex = " << __strIndex << std::endl;

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

	if(*__it == (int) MoguSyntax::TOKEN_DELIM && __strIndex < (int) __strTokens.size()-1)
	   __strIndex++;	

	if(__delStringCount >= 0 && *__it == (int) MoguSyntax::TOKEN_DELIM )
		__delStringCount++;

	std::cout << "next(): currentToken = " << currentToken<int>() << 
		" || strIndex = " << __strIndex << std::endl;
}

void TokenManager::prev()
{
	if(__delStringCount >= 0 && *__it == (int) MoguSyntax::TOKEN_DELIM )
		__delStringCount--;

	__it--;

	if(*__it == (int) MoguSyntax::TOKEN_DELIM && __strIndex > 0)
		__strIndex--;

	
	std::cout << "prev(): currentToken = " << currentToken<int>() << 
		" || strIndex = " << __strIndex << std::endl;
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
	if(*__it == (int) MoguSyntax::TOKEN_DELIM)
		__strIndex++;
	__delStringCount = -1;
}

void TokenManager::deleteFromSaved()
{
	std::cout << "deleteFromSaved(BEFORE): strIndex=" << __strIndex
		<< "  delStringCount=" << __delStringCount << std::endl;

	__it = __numTokens.erase(__savedit, __it+1);
	__strIndex -= __delStringCount;
	if(*__it == (int) MoguSyntax::TOKEN_DELIM)
		__strIndex++;
	__delStringCount = -1;

	std::cout << "deleteFromSaved(AFTER): strIndex=" << __strIndex
		<< "  delStringCount=" << __delStringCount << std::endl;
}

//call this directly after deleteToSaved()!
void TokenManager::injectToken(int numToken)
{
	__it = __numTokens.insert(__it, numToken);
}

//call this directly after deleteToSaved()!
void TokenManager::injectToken(std::string strToken)
{
	std::cout << "injectToken(strToken)" << std::endl;
	__it = __numTokens.insert(__it, (int) MoguSyntax::TOKEN_DELIM);
	std::cout << "hi" << std::endl;
	std::cout << __FUNCTION__ << "(): strIndex=" << __strIndex << " strSize=" << __strTokens.size() << std::endl;
	std::cout << "blarr" << std::endl;
	__strTokens.insert(__strTokens.begin()+__strIndex+1, strToken);
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

void TokenManager::printNumTokens()
{
	std::cout << "[TM numTokens]: ";
	for(unsigned int i=0; i<__numTokens.size(); i++)
		std::cout << __numTokens[i] << " ";
	std::cout << std::endl;
}

void TokenManager::printStringTokens()
{
	std::cout << "[TM strTokens]: ";
	for(unsigned int i=0; i<__strTokens.size(); i++)
		std::cout << i << ":" << __strTokens[i];
	std::cout << std::endl;
}


}	// namespace Parsers
