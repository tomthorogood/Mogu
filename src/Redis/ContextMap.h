#ifndef CONTEXTMAP_H_
#define CONTEXTMAP_H_

#include "../Config/typedefs.h"

struct ContextMap {
    Redis::Context*  widgets      = nullptr;
    Redis::Context*  user         = nullptr;
    Redis::Context*  meta         = nullptr;
    Redis::Context*  validators   = nullptr;
    Redis::Context*  policies     = nullptr;
    Redis::Context*  data         = nullptr;
    Redis::Context*  group        = nullptr;
    Redis::Context*  temp         = nullptr;
    Redis::Context*  perspectives = nullptr;

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
        case Prefix::temp:      return temp;
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
        case Prefix::temp:      temp = context;
                                break;
        case Prefix::perspectives:
                                perspectives = context;
                                break;
        default: break;
        }
    }

    ContextMap(){}

    ~ContextMap() {
        if (widgets != nullptr) delete widgets;
        if (user != nullptr) delete user;
        if (meta != nullptr) delete meta;
        if (validators != nullptr) delete validators;
        if (policies != nullptr) delete policies;
        if (data != nullptr) delete data;
        if (group != nullptr) delete group;
        if (temp != nullptr) delete temp;
        if (perspectives != nullptr) delete perspectives;
    }
};

#endif
