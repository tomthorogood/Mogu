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

class EventHandler : public CommandProcessor
{
public:
    EventHandler(Moldable& broadcaster, Prefix prefix, const std::string& node);

private:
    TriggerMap triggerMap;

    template <MoguSyntax T> void handleTrigger();
};

template <MoguSyntax T> void EventHandler::handleTrigger()
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
        std::cout << "Processing command: " << cmd << std::endl;
#endif
        commands.pop();
        nvp.giveInput(cmd,v);
        processCommand(v);
    }
}

#endif /* EVENTHANDLER_H_ */
