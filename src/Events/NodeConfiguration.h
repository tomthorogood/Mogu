/*
 * NodeConfiguration.h
 *
 *  Created on: Jul 11, 2012
 *      Author: tom
 */

#ifndef NODECONFIGURATION_H_
#define NODECONFIGURATION_H_

#include <declarations.h>
#include <TurnLeftLib/Utils/enumparser.h>

namespace EventNodeConfiguration
{

const int NUM_FIELDS = 8;

class NodeLabelParser
:   public TurnLeft::Utils::EnumParser <Enums::Labels::Labels>
{
public:
    NodeLabelParser()
    : TurnLeft::Utils::EnumParser <Enums::Labels::Labels> ()
    {
        using namespace Enums::Labels;
        enumMap["message"]      = message;
        enumMap["trigger"]      = trigger;
        enumMap["action"]       = action;
        enumMap["nextAction"]   = nextAction;
        enumMap["listeners"]    = listeners;
        enumMap["listener"]     = listeners;
        enumMap["degradation"]  = degradation;
        enumMap["signal"]       = signal;
        enumMap["animation"]    = animation;
        enumMap["block"]        = block;
    }
};

} //namespace EventNodeConfiguration

#endif /* NODECONFIGURATION_H_ */
