/*
 * Event_Handler.cpp
 *
 *  Created on: Apr 19, 2013
 *      Author: tom
 */


#include "EventHandler.h"
#include "TriggerMap.h"
#include "Actions.h"
#include "../Moldable/Moldable.h"
#include "../Types/CommandValue.h"

Event_Handler::Event_Handler(Moldable& w, Prefix p, const std::string& n)
    : Command_Processor(w)
    , trigger_map(broadcaster.count_triggers(), p, n)
{
    process_trigger_map();
}

Event_Handler::Event_Handler(Moldable& broadcaster, Trigger_Map& triggers)
    : Command_Processor(broadcaster), trigger_map(triggers)
{
    process_trigger_map();
}

void Event_Handler::process_trigger_map()
{
    for (int trigger : trigger_map.get_triggers())
    {
        switch (trigger)
        {
        case Mogu_Syntax::style_changed:
            broadcaster.styleChanged().connect(this,
                &Event_Handler::handle_trigger <Mogu_Syntax::style_changed.integer>);
            break;
        case Mogu_Syntax::fail:
            broadcaster.fail().connect( this,
                &Event_Handler::handle_trigger <Mogu_Syntax::fail.integer>);
            break;
        case Mogu_Syntax::succeed:
            broadcaster.succeed().connect(this,
                &Event_Handler::handle_trigger <Mogu_Syntax::succeed.integer>);
            break;
        case Mogu_Syntax::onload:
            broadcaster.onLoad().connect(this,
                &Event_Handler::handle_trigger <Mogu_Syntax::onload.integer>);
            break;
        case Mogu_Syntax::hidden_changed:
            broadcaster.hiddenChanged().connect(this,
                &Event_Handler::handle_trigger <Mogu_Syntax::hidden_changed.integer>);
            break;
        case Mogu_Syntax::index_changed:
            broadcaster.indexChanged().connect(this,
                &Event_Handler::handle_trigger <Mogu_Syntax::index_changed.integer>);
            break;
        case Mogu_Syntax::click:
            broadcaster.clicked().connect(this,
                &Event_Handler::handle_trigger <Mogu_Syntax::click.integer>);
            break;
        case Mogu_Syntax::mouseover:
            broadcaster.mouseWentOver().connect(this,
                &Event_Handler::handle_trigger <Mogu_Syntax::mouseover.integer>);
            break;
        case Mogu_Syntax::error_reported:
            broadcaster.errorReported().connect(this,
                &Event_Handler::handle_trigger <Mogu_Syntax::error_reported.integer>);
            break;
        case Mogu_Syntax::keyup:
            broadcaster.keyWentUp().connect(this,
                &Event_Handler::handle_trigger <Mogu_Syntax::keyup.integer>);
            break;
        default: continue; // just ignore bad input
        }
    }
}
void Event_Handler::process_command(Command_Value& v)
{
    switch(Mogu_Syntax::get(v.get(Command_Flags::action)))
    {
        case Mogu_Syntax::set:
            Actions::set(broadcaster,v);
            break;
        case Mogu_Syntax::increment:
            Actions::increment(broadcaster,v);
            break;
        case Mogu_Syntax::decrement:
            Actions::decrement(broadcaster,v);
            break;
        case Mogu_Syntax::test:
            Actions::test(broadcaster,v);
            break;
        case Mogu_Syntax::email:
            Actions::email(broadcaster,v);
            break;
        case Mogu_Syntax::reload:
            Actions::reload(broadcaster,v);
            break;
        case Mogu_Syntax::append:
            Actions::append(broadcaster,v);
            break;
        case Mogu_Syntax::remove:
            Actions::remove(broadcaster,v);
            break;
        case Mogu_Syntax::script:
            Actions::javascript(broadcaster,v);
            break;
        case Mogu_Syntax::clear:
            Actions::clear(broadcaster,v);
            break;
        default:return;
    }
}
