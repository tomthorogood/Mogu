/*
 * String_Map.cpp
 *
 *  Created on: Jun 18, 2013
 *      Author: tom
 */

#include "StringMap.h"

void String_Map::erase_through (size_t first, size_t last)
{
    size_t num_indexes_erased = last - first + 1;
    for (size_t i = first; i <= last; ++i) {
        if (key_exists(i)) map.erase(i);
    }

    auto iter = map.begin();
    while (iter != map.end()) {
        if (iter->first < num_indexes_erased) {
            ++iter;
            continue;
        }
        size_t new_index = iter->first - num_indexes_erased;
        size_t old_index = iter->first;
        map[new_index] = iter->second;
        map.erase(old_index);
        iter = map.begin();
    }
}

