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
: index(0)
{

}


/* Deletes from the current point THROUGH the save point, which is
 * at a greater position.
 */
void TokenManager::deleteToSaved()
{
    size_t saved_index = getIndex(savedit);
    strTokens.erase_through(index, saved_index);
	it = numTokens.erase(it, savedit+1);
	if (wasReallocated())
	    realignIterators();
	updateIndex();
}

// Deletes from the beginning of the vector to the save point.
void TokenManager::truncateHead()
{
    // NOTE: save point is deleted also
    size_t saved_index = getIndex(savedit);
    if (wasReallocated())
        realignIterators();
    it = numTokens.erase(begin_ptr, savedit+1);
    strTokens.erase_through(0, saved_index);
    updateIndex();
}
/* Deletes from the save point THROUGH the current index.
 */
void TokenManager::deleteFromSaved()
{
    size_t saved_index = getIndex(savedit);
    strTokens.erase_through(saved_index, index);
    if (it >= numTokens.end())
         it = numTokens.erase(savedit, numTokens.end());
    else
        it = numTokens.erase(savedit, it+1);
    if (wasReallocated())
        realignIterators();
    updateIndex();
}

//debug methods
void TokenManager::printTokens()
{
	std::cout << "[TokenManager]: ";
	size_t sz = numTokens.size();
	for(unsigned int i=0; i<numTokens.size(); i++)
	{
	    std::cout << "(" << i+1 << "/" << sz << "): ";
	    int token = numTokens[i];
		if(token == (int) MoguSyntax::TOKEN_DELIM)
		    std::cout << strTokens.get(i) << " ";
		else
			std::cout << numTokens[i] << " ";
	}
	std::cout << std::endl;
}

void TokenManager::printNumTokens()
{
	std::cout << "[TM numTokens]: ";
	for(unsigned int i=0; i<numTokens.size(); i++)
		std::cout << numTokens[i] << " ";
	std::cout << std::endl;
}

void TokenManager::printStringTokens()
{
	std::cout << "[TM strTokens]: ";
	for(size_t i=0; i<strTokens.size(); i++)
		std::cout << i << ":" << strTokens.get(i);
	std::cout << std::endl;
}

}	// namespace Parsers
