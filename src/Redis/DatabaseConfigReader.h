/*
 * DatabaseConfigReader.h
 *
 *  Created on: Apr 4, 2013
 *      Author: tom
 *
 *  Source: DatabaseConfigReader.cpp
 */

#ifndef DATABASECONFIGREADER_H_
#define DATABASECONFIGREADER_H_


#include "../Types/Prefixes.h"
#include <unordered_map>
#include "ContextMap.h"

#ifndef DBCONFIG_FILE //Defined at compile time or:
#define DBCONFIG_FILE "/usr/share/Mogu/dbconfig.conf"
#endif

namespace Application { //static namespace

/* Multiply the meta bit by 2, and subtract one, which effectively sums
 * all of the prefix bits.
 */
constexpr int max_prefix_mask = (((int) Prefix::meta) << 1) -1;
void load_database_contexts(Context_Map*);

namespace {
Context_Map* map_ {};

extern Context_Map* context_map() 
{
    if (!map_)
    {
        map_ = new Context_Map {};
        load_database_contexts(map_);
    }
    return map_;
}

}//namespace


} //namespace Application

#endif /* DATABASECONFIGREADER_H_ */
