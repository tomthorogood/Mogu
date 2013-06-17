/*
 * TokenManager.cpp
 *
 *  Created on: May 7th, 2013
 *      Author: cameron
 */

#include "TokenManager.h"

//debug
#include <iostream>
namespace Parsers {

TokenManager::TokenManager()
: __index(0)
{
}

void TokenManager::reset()
{
	__numTokens.clear();
	__strTokens.clear();
	__index = 0;
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

bool TokenManager::isTokenDelim()
{
	return currentToken<MoguSyntax>() == MoguSyntax::TOKEN_DELIM;
}


void TokenManager::addToken(int numToken)
{
	__numTokens.push_back(numToken);
}

void TokenManager::addToken(std::string strToken)
{
	__numTokens.push_back((int) MoguSyntax::TOKEN_DELIM);
	__strTokens.set(__numTokens.size()-1, strToken);
}

//must call this function directly after all tokens are added
//or else we will have undefined behavior!
void TokenManager::end()
{

	//place iterator at end of numerical token vector
    __it = __numTokens.end() - 1;
    updateIndex();
}

/* Like 'setiterator', but returns to the beginning, for when
 * we will not be stepping backward through the input.
 */
void TokenManager::begin() {
    __it = __numTokens.begin();
    __index = 0;
}

std::string TokenManager::fetchStringToken()
{
	if(isTokenDelim())
	{
	    /* Strip the literal quotation marks we've used as delimiters
	     * when returning
	     */
	    std::string strtoken = __strTokens.get(__index);
	    if (!isQuotedString(strtoken)) return strtoken;
	    int last_quote = strtoken.find_last_of('"');
	    return strtoken.substr(1,last_quote-1);
	}
	else
		return R"(ERR: DEREFERENCING NON-TOKENDELIM)";
}

void TokenManager::next()
{
    ++__it;
    ++__index;
}

void TokenManager::prev()
{
    --__it;
    --__index;
}

void TokenManager::saveLocation()
{
	__savedit = __it;
}

/* Deletes from the current point THROUGH the save point, which is
 * at a greater position.
 */
void TokenManager::deleteToSaved()
{
    size_t saved_index = getIndex(__savedit);
    __strTokens.erase_through(__index, saved_index);
	__it = __numTokens.erase(__it, __savedit+1);
	updateIndex();
}

// Deletes from the beginning of the vector to the save point.
void TokenManager::truncateHead()
{
    // NOTE: save point is deleted also
    size_t saved_index = getIndex(__savedit);
    __it = __numTokens.erase(__numTokens.begin(), __savedit+1);
    __strTokens.erase_through(0, saved_index);
    updateIndex();

}
/* Deletes from the save point THROUGH the current index.
 */
void TokenManager::deleteFromSaved()
{
    size_t saved_index = getIndex(__savedit);
    __strTokens.erase_through(saved_index, __index);
    if (__it >= __numTokens.end())
         __it = __numTokens.erase(__savedit, __numTokens.end());
    else
        __it = __numTokens.erase(__savedit, __it+1);
    updateIndex();
}

//call this directly after deleteToSaved()!
void TokenManager::injectToken(int numToken)
{
	__it = __numTokens.insert(__it, numToken);
	updateIndex();
}

//call this directly after deleteToSaved()!
void TokenManager::injectToken(std::string strToken)
{
	__it = __numTokens.insert(__it, (int) MoguSyntax::TOKEN_DELIM);
	__strTokens.set(__index, strToken);
    updateIndex();
}

void TokenManager::returnToSaved()
{
	__it = __savedit;
	updateIndex();
}

//debug methods
void TokenManager::printTokens()
{
	std::cout << "[TokenManager]: ";
	for(unsigned int i=0; i<__numTokens.size(); i++)
	{
		if(__numTokens[i] == (int) MoguSyntax::TOKEN_DELIM)
		    std::cout << __strTokens.get(i) << " ";
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
	for(size_t i=0; i<__strTokens.size(); i++)
		std::cout << i << ":" << __strTokens.get(i);
	std::cout << std::endl;
}


}	// namespace Parsers
