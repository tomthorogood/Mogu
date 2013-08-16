#ifndef SAFEREPLYOBJECT_H_
#define SAFEREPLYOBJECT_H_

#include <hiredis/hiredis.h>
#include <queue>
#include <string>
#include <sstream>
#include <iostream>

namespace Redis {

struct Safe_Reply_Object
{
    void* v {};
    redisReply* r {};
    std::queue <Safe_Reply_Object*> shared {};
    
    Safe_Reply_Object(){}
    Safe_Reply_Object(void* v) : r( (redisReply*)v) {sync();}
    Safe_Reply_Object(redisReply* r_) : r(r_) {}

    Safe_Reply_Object(Safe_Reply_Object& s) : shared()
    {
        s.sync();
        r = s.r;
        sync();
        shared.push(&s);
        s.shared.push(this);
    }

    Safe_Reply_Object(Safe_Reply_Object&& s) : shared()
    {
        r = s.r;
        sync();
        s.r = nullptr;
    }

    ~Safe_Reply_Object()
    {
        if (r)
        {
            freeReplyObject(r);
            while (shared.size() > 0)
            {
                Safe_Reply_Object* s = shared.front();
                shared.pop();
                s->r=nullptr;
            }
        }
    }


    operator void*() { 
        sync();
        return v;
    }
    operator void**() {
        sync();
        return &v;
    }
    operator redisReply*() { 
        sync();
        return r; 
    }
    operator bool() { 
        return ((r!=nullptr) && (r->type != REDIS_REPLY_ERROR)); 
    }

    inline int type() { 
        sync();
        return r->type; 
    }

    inline std::string str() { 
        sync();
        return r->str;
    }

    inline int integer() { 
        sync();
        return r->integer;
    }

    inline int elements()  { 
        sync();
        return r->elements; 
    }
    inline redisReply* element(size_t i) { 
        sync();
        if (i < r->elements)
            return r->element[i]; 
        return nullptr;
    }

    inline void sync() { 
        if (r && !v) 
            v = (void*)r;
        else if (v && !r)
            r = (redisReply*)v;
        else if (v != r)
        {
            r = nullptr;
            v = nullptr;
        }
    }
};

}

#endif //SAFEREPLYOBJECT_H_
