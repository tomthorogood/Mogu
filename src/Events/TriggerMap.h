/*
 * TriggerMap.h
 *
 *  Created on: Apr 19, 2013
 *      Author: tom
 */

#ifndef TRIGGERMAP_H_
#define TRIGGERMAP_H_

#include <declarations.h>
#include <set>

class TriggerMap {
public:
    TriggerMap();
    TriggerMap(const int& num_triggers,const std::string& widgetname);
    inline void addEvent(const MoguSyntax trigger, const std::string& command)
    {
        __map[trigger].push(command);
    }

    inline std::queue <std::string> getEvents(MoguSyntax trigger) const
    {
        return __map.at(trigger);
    }

    inline const std::vector <MoguSyntax>& getTriggers()
    {
        if (triggers.size() < __map.size()) populateTriggers();
        return triggers;
    }

private:
    void populateTriggers();
    std::set <MoguSyntax> triggers;
    std::unordered_map <MoguSyntax,std::queue <std::string>> __map;
};


#endif /* TRIGGERMAP_H_ */
