/*
 * EventHandler.h
 *
 *  Created on: Apr 19, 2013
 *      Author: tom
 */

#ifndef EVENTHANDLER_H_
#define EVENTHANDLER_H_

#include <Mogu.h>
#include <declarations.h>

class EventHandler
{
public:
    EventHandler(Moldable& broadcaster);

private:
    TriggerMap triggerMap;
    Moldable& broadcaster;

    template <MoguSyntax T> void handleTrigger();
    void processCommand(CommandValue& v);
};

template <MoguSyntax T> void EventHandler::handleTrigger()
{
    mApp;
    Parsers::NodeValueParser& nvp = app->interpreter();
    //COPY the queue, do not use the reference, or commands will only be fired
    //once per element!
    std::queue <std::string> commands = triggerMap.getEvents(T);
    CommandValue v;
    while (!commands.empty())
    {
        std::string cmd = commands.front();
        commands.pop();
        nvp.giveInput(cmd,v);
        processCommand(v);
    }
}

#endif /* EVENTHANDLER_H_ */
