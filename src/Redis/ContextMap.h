#ifndef CONTEXTMAP_H_
#define CONTEXTMAP_H_


#include <cassert>
#include "QueryHandler/Context.h"
#include "../Types/Prefixes.h"

struct Context_Map {
    Redis::Context*  widgets      {};
    Redis::Context*  user         {};
    Redis::Context*  meta         {};
    Redis::Context*  validators   {};
    Redis::Context*  policies     {};
    Redis::Context*  data         {};
    Redis::Context*  group        {};
    Redis::Context*  perspectives {};

    Redis::Context* get (Prefix prefix) {
        switch(prefix)
        {
        case Prefix::widgets:   return widgets;
        case Prefix::user:      return user;
        case Prefix::perspectives: return perspectives;
        case Prefix::meta:      return meta;
        case Prefix::group:     return group;
        case Prefix::validators:return validators;
        case Prefix::policies:  return policies;
        case Prefix::data:      return data;
        default: return widgets;
        }
    }

    void set(Prefix prefix, Redis::Context* context)
    {
        switch(prefix)
        {
        case Prefix::widgets:   widgets = context;
                                break;
        case Prefix::user:      user = context;
                                break;
        case Prefix::meta:      meta = context;
                                break;
        case Prefix::group:     group = context;
                                break;
        case Prefix::validators:validators = context;
                                break;
        case Prefix::policies:  policies = context;
                                break;
        case Prefix::data:      data = context;
                                break;
        case Prefix::perspectives:
                                perspectives = context;
                                break;
        default: break;
        }
    }

    Context_Map(){}

    ~Context_Map() {
        if (widgets) delete widgets;
        if (user) delete user;
        if (meta) delete meta;
        if (validators) delete validators;
        if (policies) delete policies;
        if (data) delete data;
        if (group) delete group;
        if (perspectives) delete perspectives;
    }
};

#endif
