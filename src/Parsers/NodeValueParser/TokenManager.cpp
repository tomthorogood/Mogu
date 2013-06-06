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
	__strTokens.push_back(strToken);
}

//must call this function directly after all tokens are added
//or else we will have undefined behavior!
void TokenManager::end()
{

	//place iterator at end of numerical token vector
	__it = --(__numTokens.end());

	//start our string index out-of-bounds since we update it upon
	//ENTERING a vector position which has a token delim
	__strIndex = __strTokens.size();

	//if the last element in our string vector is TOKEN_DELIM, we need
	//to update strIndex manually since next() or prev() is not called
	if(isTokenDelim())
		__strIndex--;
}

/* Like 'setiterator', but returns to the beginning, for when
 * we will not be stepping backward through the input.
 */
void TokenManager::begin() {
    __it = __numTokens.begin();
    __strIndex = -1;

}

std::string TokenManager::fetchStringToken()
{
	if(isTokenDelim())
	{
	    /* Strip the literal quotation marks we've used as delimiters
	     * when returning
	     */
	    std::string stringToken = __strTokens[__strIndex];
	    if (!isQuotedString()) return __strTokens[__strIndex];
	    int last_quote = stringToken.find_last_of('"');
	    return stringToken.substr(1,last_quote-1);
	}
	else
		return R"(ERR: DEREFERENCING NON-TOKENDELIM)";
}

bool TokenManager::isQuotedString()
{
    if (!isTokenDelim()) return false;
    std::string token = __strTokens[__strIndex];
    return token.at(0) == '"';
}

//TODO: we are dereferencing our iterator twice; once on iterator
//incr/decr, and again when/if we fetchStringToken.  refactor
void TokenManager::next()
{
	__it++;

	//if the token we just moved into is TOKEN_DELIM, we need to 
	//update strIndex to point to the right string
	if(isTokenDelim() && __strIndex < (int) __strTokens.size()-1)
	   __strIndex++;	

	//if we're between a call to saveLocation() and a subsequent call
	//to deleteFromSaved/deleteToSaved(), keep track of how many
	//strings we pass so can update strIndex accordingly when the
	//delete actually happens 
	if(__delStringCount >= 0 && isTokenDelim() )
		__delStringCount++;
}

void TokenManager::prev()
{
	//if we're between a call to saveLocation() and a subsequent call
	//to deleteFromSaved/deleteToSaved(), keep track of how many
	//strings we pass so can update strIndex accordingly when the
	//delete actually happens 
	if(__delStringCount >= 0 && isTokenDelim() )
		__delStringCount--;

	__it--;

	if(isTokenDelim()  && __strIndex > 0)
		__strIndex--;
}

void TokenManager::saveLocation()
{
	__savedit = __it;

	//begin tracking how many strings we are about to delete
	__delStringCount = 0;
	if(isTokenDelim())
		__delStringCount++;
}

void TokenManager::deleteToSaved()
{
    // Note: save point is deleted also
	__it = __numTokens.erase(__it, __savedit+1);
	
	//update strIndex as appropriate
	__strIndex -= __delStringCount;
	if(isTokenDelim())
		__strIndex++;

	//stop tracking how many strings we pass
	__delStringCount = -1;
}

// Deletes from the beginning of the vector to the save point.
void TokenManager::truncateHead()
{
    // NOTE: save point is deleted also
    __it = __numTokens.erase(__numTokens.begin(), __savedit+1);
    __strIndex -= __delStringCount;
    if (isTokenDelim())
        __strIndex++;
}

void TokenManager::deleteFromSaved()
{
	__it = __numTokens.erase(__savedit, __it+1);

	//update strIndex as appropriate
	__strIndex -= __delStringCount;
	if(isTokenDelim())
		__strIndex++;
	
	//stop tracking how many strings we pass
	__delStringCount = -1;
}

//call this directly after deleteToSaved()!
void TokenManager::injectToken(int numToken)
{
	__it = __numTokens.insert(__it, numToken);
}

//call this directly after deleteToSaved()!
void TokenManager::injectToken(std::string strToken)
{
	__it = __numTokens.insert(__it, (int) MoguSyntax::TOKEN_DELIM);
	__strTokens.insert(__strTokens.begin()+__strIndex+1, strToken);
}

void TokenManager::returnToSaved()
{
	__it = __savedit;

	//update strIndex as appropriate
	__strIndex -= __delStringCount;
	if(isTokenDelim())
		__strIndex++;
	
	//stop tracking how many strings we pass
	__delStringCount = -1;
}

//debug methods
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
