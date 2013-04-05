/*
 * DatabaseConfigReader.h
 *
 *  Created on: Apr 4, 2013
 *      Author: tom
 */

#ifndef DATABASECONFIGREADER_H_
#define DATABASECONFIGREADER_H_

#include <declarations.h>
#include <Redis/Context.h>          // Context
#include <fstream>                  // ifstream
#include <memory>                   // std::make_shared

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

enum class Prefix {
    widgets     =1,
    data        =2,
    user        =4,
    group       =8,
    templates   =16,
    validators  =32,
    policies    =64,
    /* If adding prefixes, add them BEFORE META, and UPDATE META to reflect
     * a new bit-aligned number.
     */
    meta        =128
};

namespace Application { //static namespace



std::unordered_map <Prefix, std::shared_ptr <Redis::Context> >
    contextMap;
static bool contextsLoaded = false;

/* These bits are used as flags in PREFIX_MASK to ensure
 * that an application has all database prefixes properly configured.
 */

/* Multiply the meta bit by 2, and subtract one, which effectively sums
 * all of the prefix bits.
 */
const static int MAX_PREFIX_MASK = (Prefix::meta << 1) -1;
const static int PREFIX_MASK = 0;

/* Matches the prefix passed into one of the enumeated prefixes
 * stated in the ContextManager header, and sets the corresponding
 * bit in the PREFIX_MASK so that the application can verify on startup
 * that it knows where to find everything.
 */
Prefix matchPrefix(const std::string& prefix);

int extractInteger(const std::string& line);
std::string getHost(const std::string& line);
void loadDatabaseContexts();
} //namespace Application

#endif /* DATABASECONFIGREADER_H_ */