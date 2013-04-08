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
        const char* cstr;

        inline const char* arg_c_str() {
            if (arg == EMPTY) {
                std::string itoa = std::to_string(index);
                cstr = itoa.c_str();
            }
            else cstr = arg.c_str();
            return cstr;
        }
    };
}


#endif /* FIELD_H_ */
