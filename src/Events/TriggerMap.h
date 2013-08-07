/*
 * Trigger_Map.h
 *
 *  Created on: Apr 19, 2013
 *      Author: tom
 */

#ifndef TRIGGERMAP_H_
#define TRIGGERMAP_H_

#include <declarations.h>
#include <queue>
#include <set>
#include <Types/Inthash.h>

class Trigger_Map {
public:
    Trigger_Map() {}
    Trigger_Map(const int& num_triggers,Prefix prefix, const std::string& node_name);
    
    Trigger_Map(Trigger_Map& o)
        : triggers(o.triggers)
        , map(o.map)
    {}


    Trigger_Map(Trigger_Map&& o)
        : triggers(o.triggers)
        , map(o.map)
    {
        o.triggers.clear();
        o.map.clear();
    }

    inline void add_event (const Syntax_Def& t, const std::string& c)
        { map[t].push(c); }

    inline std::queue <std::string> get_events(const int& trigger) const
        { return map.at(trigger); }

    const inline std::set <int>& get_triggers()
    { 
        if (triggers.size() < map.size()) populate_triggers();
        return trigger;
    }

    const std::unordered_map <int, std::queue <std::string>>& get_map() const
        { return map;}

private:
    void populate_triggers();
    std::set <int> triggers {}
    std::unordered_map <int, std::queue <std::string>> map {}
};


#endif /* TRIGGERMAP_H_ */
