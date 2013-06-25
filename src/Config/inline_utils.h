/*
 * inline_utils.h
 *
 *  Created on: Sep 10, 2012
 *      Author: tom
 */

#ifndef INLINE_UTILS_H_
#define INLINE_UTILS_H_

#include <sstream>
#include <Types/syntax.h>
#include <string>
#include <map>

inline std::string stripquotes(const std::string& str) {
    if (str[0] != '"') return str;
    std::string newstr = str.substr(1,str.size()-2);
    size_t escape;
    do {
        escape = newstr.find("\\\"");
        if (escape == std::string::npos) break;
        std::string temp = newstr.substr(0,escape) + newstr.substr(escape+1);
        newstr = temp;
    } while (escape != std::string::npos);
    return newstr;
}

const std::map <std::string, MoguSyntax> string_to_node_type = {
    {   "string"    ,   MoguSyntax::string  },
    {   "hash"      ,   MoguSyntax::hash    },
    {   "list"      ,   MoguSyntax::list    }
};

#endif /* INLINE_UTILS_H_ */
