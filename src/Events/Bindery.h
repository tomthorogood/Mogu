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

typedef std::vector <EventPreprocessor*> PreprocessorVector;
typedef std::map <Enums::SignalTriggers::SignalTrigger, PreprocessorVector>
	PreprocessorMap;

class EventBindery : public Wt::WObject
{

    Goo::Moldable* __broadcaster;
    PreprocessorMap __map;

    void handleVoidSignal(
            Enums::SignalTriggers::SignalTrigger trigger);

    void handleBoolSignal(
    		Enums::SignalTriggers::SignalTrigger triger, bool boolean);


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

    inline void hiddenChangedSlot(bool boolean)
    {
    	handleBoolSignal(Enums::SignalTriggers::hidden_changed, boolean);
    }

public:
    EventBindery(Goo::Moldable* broadcaster);
    ~EventBindery();
};

} //namespace Events
#endif /* BINDERY_H_ */
