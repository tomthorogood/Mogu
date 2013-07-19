/*
 * EventHandler.cpp
 *
 *  Created on: Apr 19, 2013
 *      Author: tom
 */


#include "EventHandler.h"
#include "TriggerMap.h"
#include <Types/CommandValue.h>

EventHandler::EventHandler(Moldable& broadcaster, Prefix prefix, const std::string& node)
: CommandProcessor(broadcaster), triggerMap(broadcaster.getNumTriggers(), prefix, node)
{
    processTriggerMap();
}

EventHandler::EventHandler(Moldable& broadcaster, TriggerMap& triggers)
    : CommandProcessor(broadcaster), triggerMap(triggers)
{
    processTriggerMap();
}

void EventHandler::processTriggerMap()
{
    for (int trigger : triggerMap.getTriggers())
    {
        switch (trigger)
        {
        case MoguSyntax::style_changed:
            broadcaster.styleChanged().connect(
                this,&EventHandler::handleTrigger <MoguSyntax::style_changed.integer>);
            break;
        case MoguSyntax::fail:
            broadcaster.fail().connect(
                this,&EventHandler::handleTrigger <MoguSyntax::fail.integer>);
            break;
        case MoguSyntax::succeed:
            broadcaster.succeed().connect(
                this,&EventHandler::handleTrigger <MoguSyntax::succeed.integer>);
            break;
        case MoguSyntax::onload:
            broadcaster.onLoad().connect(
                this,&EventHandler::handleTrigger <MoguSyntax::onload.integer>);
            break;
        case MoguSyntax::hidden_changed:
            broadcaster.hiddenChanged().connect(
                this,&EventHandler::handleTrigger <MoguSyntax::hidden_changed.integer>);
            break;
        case MoguSyntax::index_changed:
            broadcaster.indexChanged().connect(
                this,&EventHandler::handleTrigger <MoguSyntax::index_changed.integer>);
            break;
        case MoguSyntax::click:
            broadcaster.clicked().connect(
                this, &EventHandler::handleTrigger <MoguSyntax::click.integer>);
            break;
        case MoguSyntax::mouseover:
            broadcaster.clicked().connect(
                this, &EventHandler::handleTrigger <MoguSyntax::mouseover.integer>);
            break;
        default: continue; // just ignore bad input
        }
    }
}
