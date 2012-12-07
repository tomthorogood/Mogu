/*
 * Bindery.cpp
 *
 *  Created on: Jul 12, 2012
 *      Author: tom
 */

#include <Events/Bindery.h>
#include <Redis/RedisCore.h>

#include <Parsers/Parsers.h>
#include <Events/MoldableActionCenter.h>
#include <Events/EventPreprocessor.h>
#include <Static.h>

#include <Types/NodeValue.h>
#include <Mogu.h>

#include <Core/Moldable.h>

namespace Events {

using Goo::Moldable;
using std::string;
using Redis::strvector;
namespace Labels = Enums::Labels;
namespace Triggers = Enums::SignalTriggers;

EventBindery::EventBindery(
    Moldable* broadcaster)
    : __map()
{
    mApp;
    __broadcaster = broadcaster;

    std::string node = broadcaster->getNode();
    node += ".events";
    app->redisExec(Mogu::Keep, "get %s", node.c_str());
    int num_events = atoi(redisReply_STRING.c_str());

    /* Then, we need to iterate through each of these events. */
    for (int e = 0; e < num_events; e++) {

        EventPreprocessor* pre = new EventPreprocessor(node, e + 1);
        __map[pre->trigger].push_back(pre);
    }

    /* Now we need to determine which event triggers are associated with this
     * node by iterating through the map. For each trigger, we will bind
     * the trigger to the handler function.
     */
    PreprocessorMap::iterator iter;
    for (iter = __map.begin(); iter != __map.end(); ++iter) {
        Triggers::SignalTrigger trigger = iter->first;

        switch (trigger) {
        case Triggers::click:
            broadcaster->clicked().connect(this, &EventBindery::clickSlot);
            break;

        case Triggers::style_changed:
            broadcaster->styleChanged().connect(this,
                &EventBindery::styleChangedSlot);
            break;
        case Triggers::mouseover:
            broadcaster->mouseWentOver().connect(this,
                &EventBindery::mouseoverSlot);
            break;
        case Triggers::mouseout:
            broadcaster->mouseWentOut().connect(this,
                &EventBindery::mouseoutSlot);
            break;
        case Triggers::fail:
            broadcaster->fail().connect(this, &EventBindery::failSlot);
            break;

        case Triggers::succeed:
            broadcaster->succeed().connect(this, &EventBindery::succeedSlot);
            break;

        case Triggers::keyup:
            broadcaster->keyWentUp().connect(this, &EventBindery::keyupSlot);
            break;

        case Triggers::enter_pressed:
            broadcaster->enterPressed().connect(this, &EventBindery::enterSlot);
            break;

        case Triggers::index_changed:
            broadcaster->stackIndexChanged().connect(this,
                &EventBindery::indexChangedSlot);
            break;

        case Triggers::hidden_changed:
            broadcaster->hiddenChanged().connect(this,
                &EventBindery::hiddenChangedSlot);
            break;

        case Triggers::onload:
            broadcaster->onLoad().connect(this, &EventBindery::onLoadSlot);
            break;

        default:
            return;
        }
    }

}

void EventBindery::handleVoidSignal(
    Triggers::SignalTrigger trigger)
{

    PreprocessorVector& vec = __map[trigger];
    size_t num_preprocs = vec.size();
#ifdef DEBUG
    std::cout << __broadcaster->getNode() << " is broadcasting a message!"
        << std::endl;
#endif
    for (size_t e = 0; e < num_preprocs; e++) {
        EventPreprocessor* p = vec[e];
#ifdef DEBUG
        std::cout << "Using preprocessor " << e;
        std::cout << " for trigger {" << trigger << "}" << std::endl;
#endif
        BroadcastMessage msg(__broadcaster, p);
        ActionCenter::submitBroadcast(msg);
    }
}

void EventBindery::handleBoolSignal(
    Triggers::SignalTrigger trigger, bool commence)
{
    if (commence) handleVoidSignal(trigger);
}

EventBindery::~EventBindery()
{
    PreprocessorMap::iterator iter = __map.begin();
    for (iter = __map.begin(); iter != __map.end(); ++iter) {
        PreprocessorVector& vec = iter->second;
        size_t size = vec.size();
        for (size_t p = 0; p < size; p++) {
            delete vec[p];
        }
    }
#ifdef DEBUG
    std::cout << "Deleting bindery for " << __broadcaster->getProperties()->node
        << std::endl;
#endif
}

}    //namespace events
