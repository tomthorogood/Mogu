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

struct BroadcastMessage
{
	Goo::Moldable* 		broadcaster;
	EventPreprocessor* 	properties;
	BroadcastMessage(Goo::Moldable*, EventPreprocessor*);
	void upgradeAction();
};


}//namespace Events

#endif /* BROADCASTMESSAGE_H_ */
