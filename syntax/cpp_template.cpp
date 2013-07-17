#ifndef SYNTAX_H_
#define SYNTAX_H_

#include <map>
#include <string>
#include <Types/SyntaxDef.h>
#include <Types/LockedMap.h>
#include <Types/NodeValue.h>
#include <stdexcept>

namespace MoguSyntax {

%(syntax_definitions)s

%(syntax_imap)s

%(syntax_smap)s

%(syntax_hmap)s

inline const SyntaxDef& get(int i)
{
    try {
        return imap[i];
    } catch (std::out_of_range& e) {
        return __NONE__;
    }
}

inline const SyntaxDef& get(const std::string& s)
{
// In production, there should never be a time
// when we look anything up by the raw human
// readable value. 
#ifdef DEBUG    
    try {
        return isdigit(s) ? smap[s] : hmap[s];
    } catch (std::out_of_range& e) {
        return __NONE__;
    }
#else
    try {
        return smap[s];
    } catch (std::out_of_range& e) {
        return __NONE__;
    }
#endif
}

inline const SyntaxDef& get(const NodeValue& v)
{
    if (v.isInt())
        return get( (int) v);
    else return get( (std::string) v);
}

} // MoguSyntax

#endif //SYNTAX_H_

