#include "TriggerMap.h"

Trigger_Map::Trigger_Map(const std::string& n, const Prefix& p)
{
    Redis::Query_Handler q {p};
    std::string prefix {prefix_map.at(p)};
    const char* c {Mogu_Syntax::get(prefix).str};

    q.append_query("llen %s.%s.events", c, n.c_str());

    int num_triggers {q.yield_response <int>()};
    q.append_query("lrange %s.%s.events 0 %d", c, n.c_str(), num_triggers);
    std::vector <std::string> v {q.yield_response<std::vector<std::string>>()};

    for (int i = 0; i < num_triggers; ++i)
    {
        std::string tr {v[i]};
        q.append_query("llen %s.%s.events.%s", c, n.c_str(), tr.c_str());
        int num_cmds {q.yield_response<int>()};
        q.append_query("lrange %s.%s.events.%s 0 %d",
            c, n.c_str(), tr.c_str(), num_cmds);
        auto vec {q.yield_response <std::vector<std::string>>()};
        int trigger {atoi(tr.c_str())};
        m[trigger] = {};
        for (std::string cmd : vec) m[trigger].push(cmd);
    }
}

void Trigger_Map::extend(const Trigger_Map& t)
{
    std::vector<int>&& v {t.get_triggers()};
    for (int i =0; i < v.size(); ++i)
    {
        if (!trigger_exists(v[i]))
        {
            m[i] = t.get_commands(v[i]);
        }
        else
        {
            std::queue <std::string> q {t.get_commands(v[i])};
            for (auto s : q) m[i].push(s);
        }
    }
}

std::vector <int> get_triggers() 
{
    std::vector <int> v {m.size(),0};
    int i {};
    auto iter = m.begin();
    while (iter != iter.end())
    {
        v[i] = iter.first;
        ++iter;
        ++i;
    }
    return v;
}
