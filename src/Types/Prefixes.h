#ifndef PREFIXES_H_
#define PREFIXES_H_

#include <unordered_map>
#include "syntax.h"

enum class Prefix {
    __NONE__    =0,
    widgets     =1,
    data        =2,
    user        =4,
    group       =8,
    templates   =16,
    validators  =32,
    policies    =64,
    perspectives=128,
    /* If adding prefixes, add them BEFORE META, and UPDATE META to reflect
     * a new bit-aligned number.
     */
    meta        =256
};

namespace {
extern const std::map <Prefix, std::string>& prefix_map()
{
    static std::map <Prefix, std::string> map_ = {
        { Prefix::widgets,      "widgets"},
        { Prefix::data,         "data"},
        { Prefix::user,         "user"},
        { Prefix::group,        "group"},
        { Prefix::templates,    "templates"},
        { Prefix::validators,   "validators"},
        { Prefix::policies,     "policies"},
        { Prefix::meta,         "meta"},
        { Prefix::perspectives, "perspectives"}
    };
    return map_;
}

extern const std::unordered_map <int, Prefix> syntax_to_prefix = {
    { (int)Mogu_Syntax::own,      Prefix::widgets },
    { (int)Mogu_Syntax::widget,   Prefix::widgets },
    { (int)Mogu_Syntax::data,     Prefix::data    },
    { (int)Mogu_Syntax::user,     Prefix::user    },
    { (int)Mogu_Syntax::group,    Prefix::group   },
    { (int)Mogu_Syntax::template_,Prefix::templates},
    { (int)Mogu_Syntax::validator,Prefix::validators},
};
}

#endif //PREFIXES_H_
