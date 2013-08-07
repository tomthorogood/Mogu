/*
 * inline_utils.h
 *
 *  Created on: Sep 10, 2012
 *      Author: tom
 */

#ifndef INLINE_UTILS_H_
#define INLINE_UTILS_H_

#include <vector>
#include <sstream>
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

inline bool is_quoted_string(const std::string& s)
{
    return (s[0]=='"') && (s[s.size()-1]=='"');
}

inline std::string sreplace(std::string str, const std::string& needle,
        std::string replacement = "")
{
    size_t needle_len = needle.size();
    size_t start_index = str.find(needle);

    while (start_index != std::string::npos)
    {
        std::string part_a = str.substr(0,start_index);
        std::string part_b = str.substr(start_index+needle_len);
        str = part_a + replacement + part_b;
        start_index = str.find(needle);    
    }
    return str;
}
/* From StackOverFlow user Evan Teran, http://stackoverflow.com/users/13430,
 * because I was * feeling super lazy.
 */
inline std::vector<std::string> &split(const std::string &s, char delim,
        std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
} 

inline bool isEmpty(std::string str)
{
    return str.empty();
}

#endif /* INLINE_UTILS_H_ */
