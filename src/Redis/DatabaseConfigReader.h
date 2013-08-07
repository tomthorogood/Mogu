/*
 * DatabaseConfigReader.h
 *
 *  Created on: Apr 4, 2013
 *      Author: tom
 */

#ifndef DATABASECONFIGREADER_H_
#define DATABASECONFIGREADER_H_


#include "../Types/Prefix.h"
#include "ContextMap.h"

#ifndef DBCONFIG_FILE //Defined at compile time or:
#define DBCONFIG_FILE "/usr/share/Mogu/dbconfig.conf"
#endif

/*! The database config file will have the following syntax:
 *      @prefix
 *          host:   [hostname|ip]
 *          port:   [int port]
 *          number: [int num]
 *
 * Example:
 *      @widgets
 *          host:   localhost
 *          port:   6379
 *          number: 0
 */


namespace Application { //static namespace


/* These bits are used as flags in PREFIX_MASK to ensure
 * that an application has all database prefixes properly configured.
 */

/* Multiply the meta bit by 2, and subtract one, which effectively sums
 * all of the prefix bits.
 */
constexpr int MAX_PREFIX_MASK = (((int) Prefix::meta) << 1) -1;

void load_database_contexts();

extern Context_Map* context_map {};

} //namespace Application

#endif /* DATABASECONFIGREADER_H_ */
