/*SN_2_0_FF_PC
 * BroadcastMessage.cpp
 *
 *  Created on: Jul 12, 2012
 *      Author: tom
 */

#include <Events/BroadcastMessage.h>
#include <Core/Moldable.h>
#include <Events/EventNodeProcessor.h>
#include <stdio.h>

namespace Events{
using std::string;
using Goo::Moldable;

namespace Listeners = Enums::Family;
namespace Action = Enums::SignalActions;
namespace Labels = Enums::Labels;
namespace SignalType = Enums::SignalTypes;
namespace TypeBit = Enums::SignalTypes;


#ifdef DEBUG
static int __BROADCAST_MESSAGE_COUNT =0;
#endif

BroadcastMessage::BroadcastMessage(
        Moldable* broadcaster, EventNodeProcessor *processor)
{
    __broadcaster = broadcaster;

    Nodes::NodeValue* incoming_message = processor->getValue(Labels::message);

#ifdef DEBUG
    std::cout << "Message ";
    std::cout << ++__BROADCAST_MESSAGE_COUNT << " from ";
    std::cout << broadcaster->getNode() << " : \"";
    std::cout << incoming_message->getString() << "\"" << std::endl;
#endif

    __interrupt = 0;

    /* Because the NodeValue is deleted when the processor is destroyed, we
     * we must gracefully get the same data into the BroadcastMessage object.
     * We do this with memcpy to keep down on the coupling, or passing of
     * arbitrary parameters into the NodeValue object. This copy will be
     * deleted once the BroadcastMessage object is destroyed.
     */
    __message = new Nodes::NodeValue(incoming_message);
    __message_type = __message->getType();


    /* Set all optional values to their null state. */
    __degradation = 0;
    __next_action = Action::NO_ACTION;
    __listener_type = Listeners::self;


    __signal_type = (unsigned char)
            processor->getValue(Labels::signal)->getInt();

    __is_pointer =
            (__signal_type & TypeBit::pointer);

    __action = (Action::SignalAction)
            processor->getValue(Labels::action)->getInt();


    if (__signal_type & TypeBit::specific_listeners)
    {
        __listener_type = (Listeners::_Family)
                processor->getValue(Labels::listeners)->getInt();
    }
    else if (__signal_type & TypeBit::registered_listener)
    {
    	__listener_name = processor->getValue(Labels::listeners)->getString();
    }

    if (__signal_type & TypeBit::signal_repeats)
    {
        __degradation = (unsigned char)
                processor->getValue(Labels::degradation)->getInt();
        if (__signal_type & TypeBit::transforms)
        {
            __next_action = (Action::SignalAction)
                    processor->getValue(Labels::nextAction)->getInt();
        }
    }

    if (processor->valueExists(Labels::interrupt))
	{
    	std::string interrupt_directive = processor->getValue(
    			Labels::interrupt)->getString();
    	if (interrupt_directive == "set") __interrupt |= INTERRUPT_CHAIN;
    	if (interrupt_directive == "clear") __interrupt |= CLEAR_INTERRUPT;
	}
}

Moldable* BroadcastMessage::getBroadcaster() const
{
    return __broadcaster;
}

unsigned char BroadcastMessage::getSignalType() const
{
    return __signal_type;
}

unsigned char BroadcastMessage::getDegradation() const
{
    return __degradation;
}

bool BroadcastMessage::isPointer() const
{
    return __is_pointer;
}

NodeValue* BroadcastMessage::getMessage() const
{
    return __message;
}

Nodes::ReadType BroadcastMessage::getMessageType() const
{
    return __message_type;
}

Action::SignalAction BroadcastMessage::getAction() const
{
    return __action;
}

Action::SignalAction BroadcastMessage::getNextAction() const
{
    return __next_action;
}

Listeners::_Family BroadcastMessage::getListenerType() const
{
    return __listener_type;
}

std::string BroadcastMessage::getListenerName() const
{
	return __listener_name;
}

void BroadcastMessage::upgradeAction()
{
    if (__signal_type & TypeBit::transforms)
    {
        __action = __next_action;
    }
}


int BroadcastMessage::degrade()
{
        return --__degradation;
}

BroadcastMessage::~BroadcastMessage()
{
	delete __message;
}

}//namespace Events

