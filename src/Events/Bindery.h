/*
 * Bindery.h
 *
 *  Created on: Jul 12, 2012
 *      Author: tom
 */

#ifndef BINDERY_H_
#define BINDERY_H_

#include <declarations.h>
#include <Wt/WObject>

namespace Events
{

typedef TurnLeft::Utils::HungryVector <EventNodeExtractor*> ExtractorVector;
typedef std::map
        <Enums::SignalTriggers::SignalTrigger, ExtractorVector> ExtractorMap;


class EventBindery : public Wt::WObject
{
    ExtractorMap extractorMap;
    Goo::Moldable* __broadcaster;

    void handleVoidSignal(
            Enums::SignalTriggers::SignalTrigger trigger);


    inline void clickSlot()
    {
    	handleVoidSignal(Enums::SignalTriggers::click);
    }

    inline void mouseoverSlot()
    {
    	handleVoidSignal(Enums::SignalTriggers::mouseover);
    }

    inline void mouseoutSlot()
    {
    	handleVoidSignal(Enums::SignalTriggers::mouseout);
    }

    inline void styleChangedSlot()
    {
    	handleVoidSignal(Enums::SignalTriggers::style_changed);
    }

    inline void keyupSlot()
    {
    	handleVoidSignal(Enums::SignalTriggers::keyup);
    }

    inline void failSlot()
    {
    	handleVoidSignal(Enums::SignalTriggers::fail);
    }

    inline void succeedSlot()
    {
    	handleVoidSignal(Enums::SignalTriggers::succeed);
    }

    inline void enterSlot()
    {
    	handleVoidSignal(Enums::SignalTriggers::enter_pressed);
    }

    inline void indexChangedSlot()
    {
    	handleVoidSignal(Enums::SignalTriggers::index_changed);
    }

    inline void debuggedSlot()
    {
    	handleVoidSignal(Enums::SignalTriggers::debugged);
    }

public:
    EventBindery(Goo::Moldable* broadcaster);
    ~EventBindery();
};

} //namespace Events
#endif /* BINDERY_H_ */
