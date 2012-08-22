/*
 * BroadcastMessage.h
 *
 *  Created on: Jul 11, 2012
 *      Author: tom
 */

#ifndef BROADCASTMESSAGE_H_
#define BROADCASTMESSAGE_H_

#include <declarations.h>
#include <Types/NodeValue.h>

namespace Events{

#define INTERRUPT_CHAIN 0x1
#define CLEAR_INTERRUPT 0x2

typedef Nodes::NodeValue NodeValue;

class BroadcastMessage
{
    Goo::Moldable* __broadcaster;
    unsigned char __signal_type;
    int __degradation;

    bool __is_pointer;

    NodeValue* __message;
    Nodes::ReadType __message_type;
    Enums::SignalActions::SignalAction __action;
    Enums::SignalActions::SignalAction __next_action;
    Enums::Family::_Family __listener_type;

    uint8_t __interrupt;
    std::string __listener_name;

public:
    BroadcastMessage(
            Goo::Moldable* broadcaster, EventNodeProcessor* processor);
    ~BroadcastMessage();

    Goo::Moldable* getBroadcaster() const;
    unsigned char getSignalType() const;
    unsigned char getDegradation() const;
    bool isPointer() const;
    NodeValue* getMessage() const;
    Nodes::ReadType getMessageType() const;
    Enums::SignalActions::SignalAction getAction() const;
    Enums::SignalActions::SignalAction getNextAction() const;
    Enums::Family::_Family getListenerType() const;
    std::string getListenerName() const;
    void upgradeAction();
    int degrade();

    inline bool interruptsChain() { return __interrupt & INTERRUPT_CHAIN;}
    inline bool clearsInterrupt() { return __interrupt & CLEAR_INTERRUPT;}
};
}//namespace Events

#endif /* BROADCASTMESSAGE_H_ */
