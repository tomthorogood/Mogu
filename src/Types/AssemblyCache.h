#ifndef ASSEMBLY_CACHE_H_
#define ASSEMBLY_CACHE_H_

#include "WidgetAssembly.h"
#include <unordered_map>
#include "../Redis/NodeEditor.h"
#include "../Types/TimeoutManager.h"
#include "Prefixes.h"

class Assembly_Cache
{
public:
    Assembly_Cache()
    {}

    virtual ~Assembly_Cache() { clear(); }

    inline void add(Widget_Assembly& w) { m[w.node] = w; }

    virtual inline Widget_Assembly& get(const std::string& n) { return m[n]; }

    inline void remove(const std::string& n)
    { 
        m.erase(n);
    }

    inline virtual bool cached(const std::string& n)
        { return static_cast<bool>(m.count(n)); }
    
    inline void clear() { m.clear(); }

protected:
    std::unordered_map <std::string, Widget_Assembly> m {};
};

class Timed_Assembly_Cache : public Assembly_Cache
{
public:
    Timed_Assembly_Cache() 
        : Assembly_Cache()
        , update_code(get_update_code()) 
    {}

    Timed_Assembly_Cache(const time_t& t) 
        : Assembly_Cache()
        , update_code(get_update_code())
        , timer(t)
    {}

    inline virtual bool cached (const std::string& n) override
    {
        if (timer.expired() && database_was_updated()) clear();
        return Assembly_Cache::cached(n);
    }
private:

    inline std::string get_update_code()
    {
        Node_Value v {"update_code"};
        Redis::Node_Editor e {Prefix::meta, "cache", &v};
        return e.read();
    }

    inline bool database_was_updated()
    {
        std::string s {get_update_code()};
        if (s!=update_code)
        {
            update_code = s;
            return true;
        }
        return false;
    }

    std::string update_code {};
    Timeout_Manager timer {};
};
#endif
