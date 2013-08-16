#ifndef SYNTAX_H_
#define SYNTAX_H_

#include <map>
#include <string>
#include "SyntaxDef.h"
#include "LockedMap.h"
#include "NodeValue.h"
#include <stdexcept>

namespace Mogu_Syntax {

%(syntax_definitions)s

%(syntax_imap)s

%(syntax_smap)s

%(syntax_hmap)s

inline const Syntax_Def& get(int i)
{
    try {
        return imap[i];
    } catch (std::out_of_range& e) {
        return __NONE__;
    }
}

inline const Syntax_Def& get(const std::string& s)
{
    try {
        return isdigit(s[0]) ? smap[s] : hmap[s];
    } catch (std::out_of_range& e) {
        return __NONE__;
    }
}

inline const Syntax_Def& get(const Node_Value& v)
{
    if (v.is_int())
        return get( (int) v);
    else return get( (std::string) v);
}

} // Mogu_Syntax

#endif //SYNTAX_H_

