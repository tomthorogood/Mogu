/*
 * Bindery.cpp
 *
 *  Created on: Jul 12, 2012
 *      Author: tom
 */

#include <Events/Bindery.h>
#include <Redis/RedisCore.h>

#include <Parsers/NodeValueParser.h>
#include <Parsers/Parsers.h>

#include <Events/NodeConfiguration.h>
#include <Events/EventNodeExtractor.h>
#include <Events/EventNodeProcessor.h>
#include <Events/MoldableActionCenter.h>
#include <Static.h>

#include <Types/NodeValue.h>

#include <Core/Moldable.h>

namespace Events
{

using Goo::Moldable;
using std::string;
using Parsers::NodeValueParser;
using Redis::strvector;
namespace Labels = Enums::Labels;
namespace Triggers = Enums::SignalTriggers;


EventBindery::EventBindery(Moldable* broadcaster)
: extractorMap()
{
    __broadcaster = broadcaster;
    string eventNamespace = broadcaster->getNode();
    eventNamespace.append(".events.*");
    strvector eventNodes;
    Redis::command("keys", eventNamespace);
    /* This returns a list of all events for the widget in question. */
    Redis::toVector(eventNodes);

    int num_events = eventNodes.size();
    /* Then, we need to iterate through each of these events. */
    for (int e = 0; e < num_events; e++)
    {
        string* eventNode = &eventNodes.at(e);

        /* This will extract all values from the event node. */
        EventNodeExtractor* extractor = new EventNodeExtractor(*eventNode);

        /* Get the trigger string extracted from the node. */
        string trigger_str = extractor->getValue(Labels::trigger);

        /* Parse the trigger string for its enumerated type. */
        Nodes::NodeValue val;
        NodeValueParser nodeParser(trigger_str, &val,
                broadcaster,
                &Parsers::enum_callback<Parsers::SignalTriggerParser>
        );
        Triggers::SignalTrigger trigger = (Triggers::SignalTrigger)
                val.getInt();

        /* Add an entry to the extractorMap which binds the enumerated
         * trigger to a vector of extractors.
         */
        ExtractorMap::iterator iter = extractorMap.find(trigger);

        /* If the trigger has not already been added to the map, add it, and
         * give the associated vector some room to work with.
         */
        if (iter == extractorMap.end())
        {
            extractorMap[trigger].reserve(1);
            extractorMap[trigger][0] = extractor;
        }
        else
        {

        /* Finally, add the extractor created into the vector associated with
         * the trigger.
         */
        	extractorMap[trigger].push_back(extractor);
        }
    }

    /* Now we need to determine which event triggers are associated with this
     * node by iterating through the map. For each trigger, we will bind
     * the trigger to the handler function.
     */
    ExtractorMap::iterator iter;
    for (iter = extractorMap.begin(); iter != extractorMap.end(); iter++)
    {
        Triggers::SignalTrigger trigger = iter->first;

        switch(trigger)
        {
        case Triggers::click:
            broadcaster->clicked().connect(this,
                    &EventBindery::clickSlot);
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
        case Triggers::fail:{
        	broadcaster->fail().connect(this, &EventBindery::failSlot);
        	break;}

        case Triggers::succeed:{
        	broadcaster->succeed().connect(this, &EventBindery::succeedSlot);
        	break;}

        case Triggers::keyup:{
        	broadcaster->keyWentUp().connect(this, &EventBindery::keyupSlot);
        	break;}

        case Triggers::enter_pressed:{
        	broadcaster->enterPressed().connect(
        			this, &EventBindery::enterSlot);
        	break;}

        case Triggers::index_changed:{
        	broadcaster->stackIndexChanged().connect(
        			this, &EventBindery::indexChangedSlot);
        }
        case Triggers::debugged:{
        	broadcaster->debugged().connect(
        			this, &EventBindery::debuggedSlot);
        	break;}
        default:return;
        }
    }

}

void EventBindery::handleVoidSignal(Triggers::SignalTrigger trigger)
{
   ExtractorVector* extractors = &extractorMap[trigger];
   int num_extractors = extractors->size();
   for (int e = 0; e < num_extractors; e++)
   {
       EventNodeExtractor* extractor = extractors->at(e);
       ActionCenter::submitBroadcast(
               ActionCenter::generateNewBroadcast(
                       __broadcaster, *extractor)
       );
   }
}

EventBindery::~EventBindery()
{
	ExtractorMap::iterator iter = extractorMap.begin();
	while (iter != extractorMap.end())
	{
		ExtractorVector* vec = &iter->second;
		unsigned int size = vec->size();
		for (unsigned int p = 0; p < size; p++)
		{
			delete vec->at(p);
		}
		++iter;
	}
#ifdef DEBUG
	std::cout << "Deleting bindery for " <<
			__broadcaster->getProperties()->node << std::endl;
#endif
}

}//namespace events
