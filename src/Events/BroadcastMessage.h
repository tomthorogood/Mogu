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

namespace Events {

typedef NodeValue NodeValue;

struct BroadcastMessage
{
    Moldable* broadcaster =nullptr;
    std::vector <Listener*> listeners;
    EventPreprocessor* properties;
    BroadcastMessage(
        Moldable*, EventPreprocessor*);
    void updateAction();
    ~BroadcastMessage();
    void resolveListeners();
};

}    //namespace Events

#endif /* BROADCASTMESSAGE_H_ */
