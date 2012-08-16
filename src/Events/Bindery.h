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

    void clickSlot();
    void mouseoverSlot();
    void mouseoutSlot();
    void styleChangedSlot();
    void failSlot();
    void handleVoidSignal(
            Enums::SignalTriggers::SignalTrigger trigger);

public:
    EventBindery(Goo::Moldable* broadcaster);
    ~EventBindery();
};

} //namespace Events
#endif /* BINDERY_H_ */
