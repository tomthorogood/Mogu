#include "TriggerMap.h"
#include "../Types/KeyGenerator.h"

Trigger_Map::Trigger_Map(const std::string& n, const Prefix& p)
{
    Redis::Mogu_Query_Handler q {p};
    std::string prefix {prefix_to_string(p)};
    const char* c {prefix.c_str()};
    q.append_query("llen %s.%s.events", c, n.c_str());

    int num_triggers {q.yield_response <int>()};
    q.append_query("lrange %s.%s.events 0 %d", c, n.c_str(), num_triggers);
    std::vector <std::string> v {q.yield_response<std::vector<std::string>>()};

    Events::Trigger_Map_Info info {num_triggers, c, v, n, q};
    fill_command_map(info);
}

void Trigger_Map::fill_command_map(Events::Trigger_Map_Info& i)
{
    for (int j = 0; j < i.num_triggers; ++j)
    {
        std::string tr {i.v[j]};
        
        i.q.append_query("llen %s.%s.events.%s"
                , i.prefix.c_str(), i.node.c_str(), tr.c_str());
        
        int num_cmds {i.q.yield_response<int>()};
        
        i.q.append_query("lrange %s.%s.events.%s 0 %d",
            i.prefix.c_str(), i.node.c_str(), tr.c_str(), num_cmds);
        
        std::vector <std::string> vec 
            {i.q.yield_response <std::vector<std::string>>()};
        
        int trigger {atoi(tr.c_str())};

        if (!m.count(trigger))
            m[trigger] = {};

        for (std::string cmd : vec)
            m[trigger].push(cmd);
    }
}

Trigger_Map& Trigger_Map::extend(const Trigger_Map& t)
{
    Key_Generator <int> triggers {t.peek()};
    while (triggers)
    {
        int trigger {triggers.yield()};

        // If the trigger doesn't exist in this queue,
        // pull it from the other one.
        if (!trigger_exists(trigger))
        {
            m[trigger] = t.get_commands(trigger);
        }
        else
        {
            auto q(t.get_commands(trigger));
            while (q.size())
            {
                m[trigger].push(q.front());
                q.pop();
            }
        }
    }
    return *this;
}
