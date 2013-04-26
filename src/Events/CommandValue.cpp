/*
 * CommandValue.cpp
 *
 *  Created on: Apr 19, 2013
 *      Author: tom
 */

#include "CommandValue.h"
#include <Moldable/Moldable.h>
#include <Types/NodeValue.h>

CommandValue::CommandValue(Moldable& widget)
: broadcaster(widget), identifier(), arg(), value()
{

}

