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
    inline void addEvent(const SyntaxDef& trigger, const std::string& command)
    {
        __map[trigger].push(command);
    }

    inline std::queue <std::string> getEvents(const SyntaxDef& trigger) const
    {
        return __map.at(trigger);
    }

    inline const std::set <const SyntaxMap&>& getTriggers()
    {
        if (triggers.size() < __map.size()) populateTriggers();
        return triggers;
    }

private:
    void populateTriggers();
    std::set <const SyntaxDef&> triggers;
    std::unordered_map <const SyntaxDef& ,std::queue <std::string>, IntHash<const SyntaxDef&>> __map;
};


#endif /* TRIGGERMAP_H_ */
