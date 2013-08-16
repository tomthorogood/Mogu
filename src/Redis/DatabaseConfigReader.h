/*
 * DatabaseConfigReader.h
 *
 *  Created on: Apr 4, 2013
 *      Author: tom
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

void load_database_contexts();

} //namespace Application

#endif /* DATABASECONFIGREADER_H_ */
