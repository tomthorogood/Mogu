#ifndef PREFIXES_H_
#define PREFIXES_H_

#include <unordered_map>
#include "syntax.h"

enum class Prefix {
    __NONE__    =0,
    widget      =1,
    data        =2,
    user        =4,
    group       =8,
    template_   =16,
    validator   =32,
    policy      =64,
    perspective =128,
    /* If adding prefixes, add them BEFORE META, and UPDATE META to reflect
     * a new bit-aligned number.
     */
    meta        =256
};

namespace {

extern const std::map <Prefix, std::string>& prefix_to_string()
{
    static std::map <Prefix, std::string> map_ = {
        { Prefix::widget ,      "widget"},
        { Prefix::data,         "data"},
        { Prefix::user,         "user"},
        { Prefix::group,        "group"},
        { Prefix::template_,    "template"},
        { Prefix::validator,    "validator"},
        { Prefix::policy,       "policy"},
        { Prefix::meta,         "meta"},
        { Prefix::perspective,  "perspective"}
    };
    return map_;
}

extern const std::string& prefix_to_string(const Prefix& p)
{
    return prefix_to_string().at(p);
}

extern const std::unordered_map<int,Prefix>& syntax_to_prefix() {
    static std::unordered_map <int, Prefix> m {
        { (int)Mogu_Syntax::own,      Prefix::widget  },
        { (int)Mogu_Syntax::widget,   Prefix::widget  },
        { (int)Mogu_Syntax::data,     Prefix::data    },
        { (int)Mogu_Syntax::user,     Prefix::user    },
        { (int)Mogu_Syntax::group,    Prefix::group   },
        { (int)Mogu_Syntax::template_,Prefix::template_},
        { (int)Mogu_Syntax::validator,Prefix::validator},
    };
    return m;
}

extern const Prefix& syntax_to_prefix (const Syntax_Def& s)
{
    return syntax_to_prefix().at((int)s);
}

extern const Prefix& syntax_to_prefix (const int& i)
{
    return syntax_to_prefix().at(i);
}

extern const std::map <Prefix, int> prefix_to_syntax() {
    static std::map <Prefix, int> m {
        { Prefix::widget,   (int) Mogu_Syntax::widget   },
        { Prefix::data,     (int) Mogu_Syntax::data     },
        { Prefix::user,     (int) Mogu_Syntax::user     },
        { Prefix::group,    (int) Mogu_Syntax::group    },
        { Prefix::template_,(int) Mogu_Syntax::template_},
        { Prefix::validator,(int) Mogu_Syntax::validator}
    };
    return m;
}

extern const int& prefix_to_syntax(const Prefix& p)
{
    return prefix_to_syntax().at(p);
}

}//namespace


#endif //PREFIXES_H
