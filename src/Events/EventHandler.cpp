/*
 * EventHandler.cpp
 *
 *  Created on: Apr 19, 2013
 *      Author: tom
 */


#include "EventHandler.h"

EventHandler::EventHandler(Moldable& broadcaster)
: triggerMap(broadcaster.getNumTriggers(), broadcaster.getNode()),
  broadcaster(broadcaster)
{
    for (auto trigger : triggerMap.getTriggers())
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
        default: continue; // just ignore bad input
        }
    }
}

void EventHandler::processCommand(CommandValue& v)
{
    switch(v.getAction())
    {
        case MoguSyntax::set:                   //✓
            Actions::set(broadcaster,v);
            break;
        case MoguSyntax::increment:             //✓
            Actions::increment(broadcaster,v);
            break;
        case MoguSyntax::decrement:             //✓
            Actions::decrement(broadcaster,v);
            break;
        case MoguSyntax::test:                  //✓ 
            Actions::test(broadcaster,v);
            break;
        case MoguSyntax::email: 
//          Actions::email(broadcaster,v);
            break;
        case MoguSyntax::reload:                //✓
            Actions::reload(broadcaster,v);
            break;
        case MoguSyntax::append:                //✓
            Actions::append(broadcaster,v);
            break;
        case MoguSyntax::reset:                 //✓
            Actions::reset(broadcaster,v);
            break;
        case MoguSyntax::remove:
            Actions::remove(broadcaster,v);     //✓
            break;
        case MoguSyntax::javascript:            //✓
            Actions::javascript(broadcaster,v);
            break;
        default:return;
    }
}

