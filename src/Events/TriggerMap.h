#ifndef TRIGGER_MAP_H_
#define TRIGGER_MAP_H_

#include <unordered_map>
#include <queue>
#include <string>

class Trigger_Map
{
    using Command_Queue = std::queue<std::string>;

public:
    Trigger_Map () {}
    Trigger_Map (const std::string& node, const Prefix& p);
    Trigger_Map (const Trigger_Map& t) : m(t.m) {}
    Trigger_Map (const Trigger_Map && t) : m(t.m) { t.m.clear(); }

    void extend (const Trigger_Map&);

    inline Command_Queue get_commands(const int& i) { return m.at(i); }
    inline bool trigger_exists (const int& i)
        { return static_cast<bool>(m.count(i)); }

    inline std::vector <int> get_triggers();

private:
    std::unordered_map <int, Command_Queue> m {};
};

#endif
