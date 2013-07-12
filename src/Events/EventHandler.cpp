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
    for (MoguSyntax trigger : triggerMap.getTriggers())
    {
        switch (trigger)
        {
        case MoguSyntax::style_changed:
            broadcaster.styleChanged().connect(
                this,&EventHandler::handleTrigger <MoguSyntax::style_changed>);
            break;
        case MoguSyntax::fail:
            broadcaster.fail().connect(
                this,&EventHandler::handleTrigger <MoguSyntax::fail>);
            break;
        case MoguSyntax::succeed:
            broadcaster.succeed().connect(
                this,&EventHandler::handleTrigger <MoguSyntax::succeed>);
            break;
        case MoguSyntax::onload:
            broadcaster.onLoad().connect(
                this,&EventHandler::handleTrigger <MoguSyntax::onload>);
            break;
        case MoguSyntax::hidden_changed:
            broadcaster.hiddenChanged().connect(
                this,&EventHandler::handleTrigger <MoguSyntax::hidden_changed>);
            break;
        case MoguSyntax::index_changed:
            broadcaster.indexChanged().connect(
                this,&EventHandler::handleTrigger <MoguSyntax::index_changed>);
            break;
        case MoguSyntax::click:
            broadcaster.clicked().connect(
                this, &EventHandler::handleTrigger <MoguSyntax::click>);
            break;
        case MoguSyntax::mouseover:
            broadcaster.clicked().connect(
                this, &EventHandler::handleTrigger <MoguSyntax::mouseover>);
            break;
        default: continue; // just ignore bad input
        }
    }
}

