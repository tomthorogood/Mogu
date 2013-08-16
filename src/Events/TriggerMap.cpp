/*
 * Trigger_Map.cpp
 *
 *  Created on: Apr 19, 2013
 *      Author: tom
 */

#include "TriggerMap.h"
#include <Redis/MoguQueryHandler.h>
#include <Redis/DatabaseConfigReader.h>


Trigger_Map::Trigger_Map(const int& i, Prefix p, const std::string& n)
:map()
{
    Redis::Mogu_Query_Handler q {p};
    std::string g {prefix_map().at(p)};

    q.append_query("lrange %s.%s.events 0 %d", g.c_str(), n.c_str(), i);

    /* Retrieve the list of all event triggers for the widget
     * and for each of those retrieve the list of commands, appending
     * the commands to the trigger's command queue
     */
    for (std::string s : q.yield_response <std::vector <std::string>>())
    {

        //Convert "12" to 12
        const char* c = s.c_str();
        int t = atoi(c);
        triggers.insert(Mogu_Syntax::get(t));
        
        // Get the number of commands associated with that trigger
        q.append_query("llen %s.%s.events.%d", g.c_str(), n.c_str(), t);
        
        int r = q.yield_response<int>();
        q.append_query("lrange %s.%s.events.%d 0 %d", g.c_str(), n.c_str(),t,r);

        // Store the commands in the trigger queue
        for (std::string cmd : q.yield_response <std::vector <std::string>>())
        {
            map[(int)Mogu_Syntax::get(t)].push(cmd);
        }
    }
}

void Trigger_Map::extend_map(std::unordered_map <int, std::queue <std::string>> m)
{
    for (auto i : m)
    {
        int t {i.first};
        std::queue<std::string> q {i.second};
        while (i.second.size())
        {
            std::string s {q.front()};
            q.pop();
            map[t].push(s);
        }
    }
}

void Trigger_Map::populate_triggers()
{
    for (auto i = map.begin(); i != map.end(); ++i)
    {
        triggers.insert(i->first);
    }
}
