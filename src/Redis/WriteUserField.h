/*
 * WriteUserField.h
 *
 *  Created on: Apr 8, 2013
 *      Author: tom
 */

#ifndef WRITEUSERFIELD_H_
#define WRITEUSERFIELD_H_

#include <declarations.h>

namespace Redis {

void writeUserField(
        const std::string& keyspace
        , const Field& field
        , const NodeValue& value);

}//namespace Redis


#endif /* WRITEUSERFIELD_H_ */
