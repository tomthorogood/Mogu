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

typedef NodeValue NodeValue;

struct BroadcastMessage
{
	Goo::Moldable* broadcaster;
	Listeners listeners;
	EventPreprocessor* 	properties;
	BroadcastMessage(Goo::Moldable*, EventPreprocessor*);
	void updateAction();
	~BroadcastMessage();
	void resolveListeners();
};


}//namespace Events

#endif /* BROADCASTMESSAGE_H_ */
