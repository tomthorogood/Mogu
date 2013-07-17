/*
 * DatabaseConfigReader.h
 *
 *  Created on: Apr 4, 2013
 *      Author: tom
 */

#ifndef DATABASECONFIGREADER_H_
#define DATABASECONFIGREADER_H_

#include <declarations.h>
#include <Types/Inthash.h>
#include <Redis/Context.h>          // Context
#include <fstream>                  // ifstream
#include <memory>                   // std::make_shared
#include <cassert>


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


const std::unordered_map <Prefix, std::string, IntHash<Prefix>> prefixMap = {
    { Prefix::widgets,      "widgets"},
    { Prefix::data,         "data"},
    { Prefix::user,         "user"},
    { Prefix::group,        "group"},
    { Prefix::templates,    "templates"},
    { Prefix::validators,   "validators"},
    { Prefix::policies,     "policies"},
    { Prefix::meta,         "meta"},
    { Prefix::temp,         "temp"},
    { Prefix::perspectives, "perspectives"}
};

const std::unordered_map <int, Prefix> syntax_to_prefix = {
    { (int)MoguSyntax::widget,   Prefix::widgets },
    { (int)MoguSyntax::data,     Prefix::data    },
    { (int)MoguSyntax::user,     Prefix::user    },
    { (int)MoguSyntax::group,    Prefix::group   },
    { (int)MoguSyntax::template_,Prefix::templates},
    { (int)MoguSyntax::validator,Prefix::validators},
};

namespace Application { //static namespace


/* These bits are used as flags in PREFIX_MASK to ensure
 * that an application has all database prefixes properly configured.
 */

/* Multiply the meta bit by 2, and subtract one, which effectively sums
 * all of the prefix bits.
 */
const static int MAX_PREFIX_MASK = (((int) Prefix::meta) << 1) -1;

/* Matches the prefix passed into one of the enumeated prefixes
 * stated in the ContextManager header, and sets the corresponding
 * bit in the PREFIX_MASK so that the application can verify on startup
 * that it knows where to find everything.
 */
Prefix matchPrefix(const std::string& prefix);

int extractInteger(const std::string& line);
std::string getHost(const std::string& line);
ContextMap* loadDatabaseContexts();
} //namespace Application

#endif /* DATABASECONFIGREADER_H_ */
