/*
 * Event_Handler.h
 *
 *  Created on: Apr 19, 2013
 *      Author: tom
 */

#ifndef EVENTHANDLER_H_
#define EVENTHANDLER_H_

#include <Mogu.h>
#include <Parsers/Node_Value_Parser.h>
#include "TriggerMap.h"
#include <Types/MoguLogger.h>

class Event_Handler : public Command_Processor
{
public:
    Event_Handler(Moldable& broadcaster, Prefix, const std::string& node);
    Event_Handler(Moldable& broadcaster, Trigger_Map&);

    virtual ~Event_Handler(){}

private:
    Trigger_Map trigger_map {};
    void process_trigger_map();
    template <const int> void handle_trigger();
    void process_command(Command_Value&);
};

template <const int T> void Event_Handler::handle_trigger()
{
    static int cmd_count =0;
    mApp;
    Parsers::Node_Value_Parser& nvp = app->interpreter();
    //COPY the queue, do not use the reference, or commands will only be fired
    //once per element!
    std::queue <std::string> q = trigger_map.get_events(T);

    while (!q.empty())
    {
        Command_Value v(broadcaster);
        std::string cmd = commands.front();
        Application::log.log(
                Log_Level::NOTICE, "Processing command ",
                cmd_count++, ": ", cmd);
        commands.pop();
        nvp.give_input(cmd,v,&broadcaster);
        process_command(v);
    }
}

#endif /* EVENTHANDLER_H_ */
