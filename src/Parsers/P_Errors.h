/*
 * P_Errors.h
 *
 *  Created on: Aug 18, 2012
 *      Author: tom
 */

#ifndef P_ERRORS_H_
#define P_ERRORS_H_

#include <declarations.h>

namespace Parsers{
class HashableAnswer_ErrorFlagParser
{
    std::map
        <char, Enums::HashableAnswer_ErrorFlags::HashableAnswer_ErrorFlags>
            enumMap;
public:
    HashableAnswer_ErrorFlagParser()
    {
    	namespace Flag = Enums::HashableAnswer_ErrorFlags;
		enumMap[1]                  = Flag::err_no_hashable_data;
		enumMap[3]                  = Flag::err_no_hashable_question;
		enumMap[5]                  = Flag::err_no_hashable_question;
		enumMap[7]                  = Flag::err_no_hashable_question;
		enumMap[9]                  = Flag::err_no_hashable_form;
		for (unsigned char c = 11; c < 33; c+=2)
		{
			enumMap[c]              = Flag::err_no_hashable_answer;
		}
		enumMap[33]                 = Flag::err_no_hashable_form;
		enumMap[35]                 = Flag::err_no_hashable_question;
		enumMap[37]                 = Flag::err_no_hashable_question;
		enumMap[39]                 = Flag::err_no_hashable_question;
		enumMap[41]                 = Flag::err_no_hashable_form;
		for (unsigned char c = 43; c < 65; c+=2)
		{
			enumMap[c]              = Flag::is_hashable;
		}
		enumMap[65]                 = Flag::err_hash_already_set;
		enumMap[129]                = Flag::err_unknown;
    }
    Enums::HashableAnswer_ErrorFlags::HashableAnswer_ErrorFlags
        parse(unsigned char& ch)
    {
        if (ch % 2 == 0) return
                Enums::HashableAnswer_ErrorFlags::err_no_database_selected;
        if (ch >129) return enumMap[129];
        if (ch >65) return enumMap[65];
        return enumMap[ch];
    }
};

}//namespace Parsers


#endif /* P_ERRORS_H_ */
