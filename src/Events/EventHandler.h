/*
 * EventHandler.h
 *
 *  Created on: Apr 19, 2013
 *      Author: tom
 */

#ifndef EVENTHANDLER_H_
#define EVENTHANDLER_H_

#include <declarations.h>
#include <Mogu.h>
#include <Parsers/NodeValueParser.h>
#include "TriggerMap.h"
#include "CommandProcessor.h"

#ifdef DEBUG
    static int cmd_count =0;
#endif

class EventHandler : public CommandProcessor
{
public:
    EventHandler(Moldable& broadcaster, Prefix prefix, const std::string& node);
    EventHandler(Moldable& broadcaster, TriggerMap& triggers);
    ~EventHandler()
    {
    }

private:
    TriggerMap triggerMap;
    /* Whether or not the TriggerMap was instantiated by this class, or
     * was passed in by an external entity.
     */
    void processTriggerMap();

    template <const int> void handleTrigger();
};

template <const int T> void EventHandler::handleTrigger()
{
    mApp;
    Parsers::NodeValueParser& nvp = app->interpreter();
    //COPY the queue, do not use the reference, or commands will only be fired
    //once per element!
    std::queue <std::string> commands = triggerMap.getEvents(T);
    while (!commands.empty())
    {
        CommandValue v(broadcaster);
        std::string cmd = commands.front();
#ifdef DEBUG
        ++cmd_count;
        std::cout << "Processing command "
            << cmd_count << ": " << cmd << std::endl;
#endif
        commands.pop();
        nvp.giveInput(cmd,v);
        processCommand(v);
    }
}

#endif /* EVENTHANDLER_H_ */
