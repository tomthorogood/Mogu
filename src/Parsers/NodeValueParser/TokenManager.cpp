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


/* Deletes from the current point THROUGH the save point, which is
 * at a greater position.
 */
void TokenManager::deleteToSaved()
{
    size_t saved_index {get_index(savedit)};
    s_tokens.erase_through(index, saved_index);
	it = i_tokens.erase(it, savedit+1);
	if (was_reallocated())
	    realign_iterators();
	update_index();
}

// Deletes from the beginning of the vector to the save point.
void TokenManager::truncate_head()
{
    // NOTE: save point is deleted also
    size_t saved_index = get_index(savedit);
    if (was_reallocated())
        realign_iterators();
    it = i_tokens.erase(begin_ptr, savedit+1);
    s_tokens.erase_through(0, saved_index);
    update_index();
}

/* Deletes from the save point THROUGH the current index.
 */
void TokenManager::deleteFromSaved()
{
    size_t saved_index {get_index(savedit)};
    s_tokens.erase_through(saved_index, index);
    if (it >= i_tokens.end())
         it = i_tokens.erase(savedit, i_tokens.end());
    else
        it = i_tokens.erase(savedit, it+1);
    if (was_reallocated())
        realign_iterators();
    update_index();
}

}	// namespace Parsers
