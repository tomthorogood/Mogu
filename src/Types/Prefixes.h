#ifndef PREFIXES_H_
#define PREFIXES_H_
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
    { (int)MoguSyntax::own,      Prefix::widgets },
    { (int)MoguSyntax::widget,   Prefix::widgets },
    { (int)MoguSyntax::data,     Prefix::data    },
    { (int)MoguSyntax::user,     Prefix::user    },
    { (int)MoguSyntax::group,    Prefix::group   },
    { (int)MoguSyntax::template_,Prefix::templates},
    { (int)MoguSyntax::validator,Prefix::validators},
};
}

#endif //PREFIXES_H_
