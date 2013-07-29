/*
 * TriggerMap.h
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

class TriggerMap {
public:
    TriggerMap();
    TriggerMap(const int& num_triggers,Prefix prefix, const std::string& node_name);
    TriggerMap(TriggerMap&);
    inline void addEvent(const SyntaxDef& trigger, const std::string& command)
    {
        map[trigger].push(command);
    }

    inline std::queue <std::string> getEvents(const int trigger) const
    {
        return map.at(trigger);
    }

    const inline std::set <int>& getTriggers()
    {
        if (triggers.size() < map.size()) populateTriggers();
        return triggers;
    }

    const std::unordered_map <int, std::queue <std::string>>& getMap() const 
    { return map; }

private:
    void populateTriggers();
    std::set <int> triggers;
    std::unordered_map <int,std::queue <std::string>> map;
};


#endif /* TRIGGERMAP_H_ */
