/*
 * Event_Handler.h
 *
 *  Created on: Apr 19, 2013
 *      Author: tom
 */

#ifndef EVENTHANDLER_H_
#define EVENTHANDLER_H_

#include "../Mogu.h"
#include "TriggerMap.h"
#include "CommandProcessor.h"
#include "../Parsers/NodeValueParser.h"
#include "../Types/MoguLogger.h"

class Event_Handler : public Command_Processor
{
public:
    Event_Handler(Moldable& broadcaster, Prefix, const std::string& node);
    Event_Handler(Moldable& broadcaster, Trigger_Map&);
    inline void set_trigger_map(Trigger_Map& t) { trigger_map = t; }

    virtual ~Event_Handler(){}

private:
    Trigger_Map trigger_map {};
    void process_trigger_map();
    template <const int> void handle_trigger();
    void process_command(Command_Value&);
};

template <const int T> void Event_Handler::handle_trigger()
{
    mApp;
    Parsers::Node_Value_Parser& nvp = app->get_interpreter();
    //COPY the queue, do not use the reference, or commands will only be fired
    //once per element!
    std::queue <std::string> q = trigger_map.get_commands(T);
    
    while (!q.empty())
    {
        Command_Value v(broadcaster);
        std::string cmd = q.front();
        q.pop();
        nvp.give_input(cmd,v,&broadcaster);
        process_command(v);
    }
}

#endif /* EVENTHANDLER_H_ */
