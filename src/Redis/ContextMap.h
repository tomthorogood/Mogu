
struct ContextMap {
    Redis::Context*  __widgets;
    Redis::Context*  __user;
    Redis::Context*  __meta;
    Redis::Context*  __validators;
    Redis::Context*  __policies;
    Redis::Context*  __data;
    Redis::Context*  __group;
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
        default: break;
        }
    }

    ContextMap(){}

    ~ContextMap() {
        delete __widgets;
        delete __user;
        delete __meta;
        delete __validators;
        delete __policies;
        delete __data;
        delete __group;
    }
};
