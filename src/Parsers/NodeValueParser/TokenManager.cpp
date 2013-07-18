/*
 * TokenManager.cpp
 *
 *  Created on: May 7th, 2013
 *      Author: cameron
 */

#include "TokenManager.h"
#include <cassert>
//debug
#include <iostream>
namespace Parsers {

TokenManager::TokenManager()
: __index(0)
{

}


/* Deletes from the current point THROUGH the save point, which is
 * at a greater position.
 */
void TokenManager::deleteToSaved()
{
    size_t saved_index = getIndex(__savedit);
    __strTokens.erase_through(__index, saved_index);
	__it = __numTokens.erase(__it, __savedit+1);
	if (wasReallocated())
	    realignIterators();
	updateIndex();
}

// Deletes from the beginning of the vector to the save point.
void TokenManager::truncateHead()
{
    // NOTE: save point is deleted also
    size_t saved_index = getIndex(__savedit);
    if (wasReallocated())
        realignIterators();
    __it = __numTokens.erase(__begin, __savedit+1);
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
    if (wasReallocated())
        realignIterators();
    updateIndex();
}

//debug methods
void TokenManager::printTokens()
{
	std::cout << "[TokenManager]: ";
	size_t sz = __numTokens.size();
	for(unsigned int i=0; i<__numTokens.size(); i++)
	{
	    std::cout << "(" << i+1 << "/" << sz << "): ";
	    int token = __numTokens[i];
		if(token == (int) MoguSyntax::TOKEN_DELIM)
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
