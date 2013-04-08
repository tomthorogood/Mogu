/*
 * FieldLookups.h
 *
 *  Created on: Apr 8, 2013
 *      Author: tom
 */

#ifndef FIELDLOOKUPS_H_
#define FIELDLOOKUPS_H_

#include <declarations.h>

namespace Redis {
void metaFieldLookup(const std::string& field, NodeValue& result);

void userFieldLookup(
    const std::string& keyspace,
    const std::string& field,
    const std::string& arg=EMPTY,
    NodeValue& result);

}//namespace Redis


#endif /* USERFIELDLOOKUP_H_ */
