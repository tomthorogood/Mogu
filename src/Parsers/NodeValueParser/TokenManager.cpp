/*
 * Token_Manager.cpp
 *
 *  Created on: May 7th, 2013
 *      Author: cameron
 */

#include "TokenManager.h"
#include <cassert>
//debug
#include <iostream>
namespace Parsers {

/* Deletes from the save point THROUGH the current index.
 */
void Token_Manager::delete_from_saved()
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
