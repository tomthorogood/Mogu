/*
 * TriggerMap.cpp
 *
 *  Created on: Apr 19, 2013
 *      Author: tom
 */

#include "TriggerMap.h"
#include <Redis/ContextQuery.h>
TriggerMap::TriggerMap() : __map() {}

TriggerMap::TriggerMap(const int& num_triggers, const std::string& widget)
:__map()
{
    Redis::ContextQuery db(Prefix::widgets);
    const char* c_node = widget.c_str();
    db.appendQuery("lrange widgets.%s.events 0 %d", c_node, num_triggers);

    /* Retrieve the list of all event triggers for the widget
     * and for each of those retrieve the list of commands, appending
     * the commands to the trigger's command queue
     */
    for (std::string s_trigger : db.yieldResponse <std::vector <std::string>>())
    {

        //Convert "12" to 12
        const char* c_trigger = s_trigger.c_str();
        int trigger = atoi(c_trigger);
        triggers.insert((MoguSyntax) trigger);
        // Get the number of commands associated with that trigger
        db.appendQuery("llen widgets.%s.events.%d", c_node, trigger);

        int mrange = db.yieldResponse <int>();

        db.appendQuery("lrange widgets.%s.events.%d 0 %d",
                c_node, trigger, mrange);


        // Store the commands in the trigger queue
        for (std::string command : db.yieldResponse <std::vector <std::string>>())
        {
            __map[(MoguSyntax) trigger].push(command);
        }
    }
}

void TriggerMap::populateTriggers()
{
    auto iter = __map.begin();
    for (size_t i = 0; i < __map.size(); ++i)
    {
        triggers.insert(iter->first);
    }
}
