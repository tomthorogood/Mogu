#ifndef SAFEREPLYOBJECT_H_
#define SAFEREPLYOBJECT_H_

#include <hiredis/hiredis.h>
#include <queue>
#include <string>
#include <sstream>
#include <iostream>

namespace {
void log(const std::string& msg) { 
#ifdef REDIS_SRO_DEBUG
    std::cout << msg << std::endl; 
#endif
}
}

template <typename T>
std::string as_string(T* ptr)
{
    std::stringstream buf;
    buf << ptr;
    return buf.str();
}

namespace Redis {

struct SafeReplyObject
{
    void* v = nullptr;
    redisReply* r = nullptr;

    std::queue <SafeReplyObject*> shared;

    SafeReplyObject(){}

    SafeReplyObject(void* v) : r( (redisReply*)v) { 
        log("SRO void* constructor");
        sync();
    }

    SafeReplyObject(redisReply* r_) : r(r_) {
        log("SRO reidsReply* constructor");
    }

    SafeReplyObject(SafeReplyObject& s) : shared()
    {
        log("SRO Copy Constructor");
        s.sync();
        log(
            "My r will now point to " + as_string(s.r));
        r = s.r;
        sync();
        shared.push(&s);
        s.shared.push(this);
    }

    SafeReplyObject(SafeReplyObject&& s) : shared()
    {
        log("SRO move constructor");
        r = s.r;
        sync();
        s.r = nullptr;
    }

    ~SafeReplyObject()
    {
        log("SRO destructor");
        if (r)
        {
            freeReplyObject(r);
            while (shared.size() > 0)
            {
                SafeReplyObject* s = shared.front();
                shared.pop();
                s->r=nullptr;
            }
        }
    }


    operator void*() { 
        log("SRO void* operator");
        sync();
        return v;
    }
    operator void**() {
        log("SRO void** operator");
        sync();
        return &v;
    }
    operator redisReply*() { 
        log ("SRO redisReply* operator");
        sync();
        return r; 
    }
    operator bool() { 
        log("SRO bool operator");
        return ((r!=nullptr) && (r->type != REDIS_REPLY_ERROR)); 
    }

    inline int type() { 
        sync();
        log("SRO type of reply at address " + as_string(r));
        return r->type; 
    }

    inline std::string str() { 
        sync();
        log("SRO str of reply at address " + as_string(r));
        return r->str;
    }

    inline int integer() { 
        sync();
        log("SRO int of reply at address " + as_string(r));
        return r->integer;
    }

    inline int elements()  { 
        sync();
        log("SRO elements of reply at address " + as_string(r));
        return r->elements; 
    }
    inline redisReply* element(int i) { 
        sync();
        log("SRO element " + std::to_string(i) + " of reply at address " 
                + as_string(r));
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
            log("SRO element pointers are corrupt. Resetting.");
            r = nullptr;
            v = nullptr;
        }
    }
};

}

#endif //SAFEREPLYOBJECT_H_
