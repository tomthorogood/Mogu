/*
 * Field.h
 *
 *  Created on: Apr 8, 2013
 *      Author: tom
 */

#ifndef FIELD_H_
#define FIELD_H_

#include <declarations.h>

namespace Redis {
    struct Field {
        std::string name =EMPTY;
        int index =0;
        std::string arg =EMPTY;
    };
}


#endif /* FIELD_H_ */
