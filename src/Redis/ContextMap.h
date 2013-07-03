#ifndef CONTEXTMAP_H_
#define CONTEXTMAP_H_

struct ContextMap {
    Redis::Context*  __widgets      = nullptr;
    Redis::Context*  __user         = nullptr;
    Redis::Context*  __meta         = nullptr;
    Redis::Context*  __validators   = nullptr;
    Redis::Context*  __policies     = nullptr;
    Redis::Context*  __data         = nullptr;
    Redis::Context*  __group        = nullptr;
    Redis::Context*  __temp         = nullptr;

    Redis::Context* get (Prefix prefix) {
        switch(prefix)
        {
        case Prefix::widgets:   return __widgets;
        case Prefix::user:      return __user;
        case Prefix::meta:      return __meta;
        case Prefix::group:     return __group;
        case Prefix::validators:return __validators;
        case Prefix::policies:  return __policies;
        case Prefix::data:      return __data;
        case Prefix::temp:      return __temp;
        default: return __widgets;
        }
    }

    void set(Prefix prefix, Redis::Context* context)
    {
        switch(prefix)
        {
        case Prefix::widgets:   __widgets = context;
                                break;
        case Prefix::user:      __user = context;
                                break;
        case Prefix::meta:      __meta = context;
                                break;
        case Prefix::group:     __group = context;
                                break;
        case Prefix::validators:__validators = context;
                                break;
        case Prefix::policies:  __policies = context;
                                break;
        case Prefix::data:      __data = context;
                                break;
        case Prefix::temp:      __temp = context;
                                break;
        default: break;
        }
    }

    ContextMap(){}

    ~ContextMap() {
        if (__widgets != nullptr) delete __widgets;
        if (__user != nullptr) delete __user;
        if (__meta != nullptr) delete __meta;
        if (__validators != nullptr) delete __validators;
        if (__policies != nullptr) delete __policies;
        if (__data != nullptr) delete __data;
        if (__group != nullptr) delete __group;
        if (__temp != nullptr) delete __temp;
    }
};

#endif
