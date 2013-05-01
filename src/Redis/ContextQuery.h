/*
 * ContextQuery.h
 *
 *  Created on: Apr 4, 2013
 *      Author: tom
 */

#ifndef CONTEXTQUERY_H_
#define CONTEXTQUERY_H_

#include "QuerySet.h"
#include <declarations.h>
#include <Redis/DatabaseConfigReader.h>
namespace Redis {

/*!\brief Abstracts the Context lookup tighten constraints
 * by which the application may interact with its set of databases.
 */

class ContextQuery : public QuerySet {

public:
    ContextQuery(Prefix);
};

}//namespace Redis

#endif /* CONTEXTQUERY_H_ */
