/*
 * TokenManager.cpp
 *
 *  Created on: May 7th, 2013
 *      Author: cameron
 */

#include <Types/syntax.h>

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
	__revit = __numTokens.rbegin();
	strIndex = __strTokens.size();	//initially points out-of-bounds
}

int TokenManager::currentToken()
{
	//internally we use a reverse iterator, but externally we are to
	//think of the tokens being arranged in the traditional forwards
	//direction
	if(__revit >= __numTokens.rend())
		return OutOfRange::Begin;

	else if(__revit < __numTokens.rbegin())
		return OutOfRange::End;

	else
		return *rit;
}



std::string TokenManager::fetchStringToken()
{
	if(*rit == (int) MoguSyntax::TOKEN_DELIM)
		return __strTokens[strIndex];

	else
		return R"(ERR: DEREFERENCING NON-TOKENDELIM");
}

//TODO: we are dereferencing our iterator twice; once on iterator
//incr/decr, and again when/if we fetchStringToken.  refactor

void TokenManager::next()
{
	rit--;
	if(*rit == (int) MoguSyntax::TOKEN_DELIM)
	   strIndex++;	
}

void TokenManager::prev()
{
	rit++;
	if(*rit == (int) MoguSyntax::TOKEN_DELIM)
		strIndex--;
}

}	// namespace Parsers
