#ifndef TRIGGER_MAP_H_
#define TRIGGER_MAP_H_

#include <unordered_map>
#include <vector>
#include <queue>
#include <string>

#include "../Types/Prefixes.h"
#include "../Redis/MoguQueryHandler.h"


namespace Events {
struct Trigger_Map_Info
{
    int num_triggers {};
    std::string prefix {};

    std::vector<std::string>& v;
    const std::string& node;
    Redis::Mogu_Query_Handler& q;

    Trigger_Map_Info(
        int i
        , std::string p
        , std::vector<std::string>& v_
        , const std::string& n
        , Redis::Mogu_Query_Handler& q_)
        : num_triggers(i)
        , prefix(p)
        , v(v_)
        , node(n)
        , q(q_)
    {}
};
}


class Trigger_Map
{
    using Command_Queue = std::queue<std::string>;

public:
    Trigger_Map ();
    Trigger_Map (const std::string& node, const Prefix& p);
    Trigger_Map (const Trigger_Map& t) : m(t.m) {}
    Trigger_Map (Trigger_Map && t) : m(t.m) { t.m.clear(); }
    Trigger_Map& extend (const Trigger_Map&);

    void fill(const std::string&, const Prefix&);

    // Note: Do NOT return a reference! The return must be independently
    // mutable in order for the Trigger Map to be reused!
    inline Command_Queue get_commands(const int& i) const { return m.at(i); }
    
    inline bool trigger_exists (const int& i)
        { return static_cast<bool>(m.count(i)); }

    inline const std::unordered_map<int, Command_Queue>& peek() const 
        { return m; }
    
    inline void clear() { m.clear(); }

    Trigger_Map& operator= (const Trigger_Map& t) { m = t.m; return *this;}

    inline size_t size() const { return m.size(); }

private:
    std::unordered_map <int, Command_Queue> m {};
    void fill_command_map(Events::Trigger_Map_Info&);
};

#endif
