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
    string eventNamespace = broadcaster->getNodeList()->at(0);
    eventNamespace.append(".events.*");
    strvector eventNodes;
    Redis::command("keys", eventNamespace);
    Redis::toVector(eventNodes);

    int num_events = eventNodes.size();

    for (int e = 0; e < num_events; e++)
    {
        string eventNode = eventNodes.at(e);
        EventNodeExtractor* extractor = new EventNodeExtractor(eventNode);
        string trigger_str = extractor->getValue(Labels::trigger);

        NodeValueParser nodeParser(trigger_str,
                broadcaster,
                &Parsers::enum_callback<Parsers::SignalTriggerParser>
        );
        Triggers::SignalTrigger trigger = (Triggers::SignalTrigger)
                nodeParser.getValue()->getInt();
        ExtractorMap::iterator iter = extractorMap.find(trigger);
        if (iter == extractorMap.end())
        {
            extractorMap[trigger].reserve(1);
        }
        extractorMap[trigger].add(extractor);
        extractorMap[trigger].trim();
        int __TEST__ = extractorMap[trigger].size();
    }

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
        default:return;
        }
    }

}

void EventBindery::handleVoidSignal(Triggers::SignalTrigger trigger)
{
   ExtractorVector extractors = extractorMap[trigger];
   int num_extractors = extractors.size();
   for (int e = 0; e < num_extractors; e++)
   {
       EventNodeExtractor* extractor = extractors.at(e);
       ActionCenter::submitBroadcast(
               ActionCenter::generateNewBroadcast(
                       __broadcaster, *extractor)
       );
   }
}

void EventBindery::clickSlot()
{
	bool debug = true;
    handleVoidSignal(Triggers::click);
}
void EventBindery::mouseoverSlot()
{
    handleVoidSignal(Triggers::mouseover);
}
void EventBindery::mouseoutSlot()
{
    handleVoidSignal(Triggers::mouseout);
}
void EventBindery::styleChangedSlot()
{
    handleVoidSignal(Triggers::style_changed);
}

}

