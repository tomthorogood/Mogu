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
    std::queue <std::string>& commands = triggerMap.getEvents(T);
    size_t sz = commands.size();
    CommandValue v;
    while (commands.size() > 0)
    {
        std::string cmd = commands.front();
        commands.pop();
        nvp.giveInput(cmd,v);
        processCommand(v);
    }
}

#endif /* EVENTHANDLER_H_ */
