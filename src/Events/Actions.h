/*
 * Actions.h
 *
 *  Created on: Apr 22, 2013
 *      Author: tom
 */
#ifndef ACTIONS_H_
#define ACTIONS_H_

#include "../Moldable/Moldable.h"
#include "../Types/CommandValue.h"

namespace Actions
{

void set(Moldable& broadcaster, Command_Value& command);
void increment(Moldable& broadcaster, Command_Value& command);
void decrement(Moldable& broadcaster, Command_Value& command);
void test(Moldable& broadcaster, Command_Value& command);
void email(Moldable& broadcaster, Command_Value& command);
void reload(Moldable& broadcaster, Command_Value& command);
void append(Moldable& broadcaster, Command_Value& command);
void reset(Moldable& broadcaster, Command_Value& command);
void remove(Moldable& broadcaster, Command_Value& command);
void javascript(Moldable& broadcaster, Command_Value& command);
void clear(Moldable& broadcaster, Command_Value& command);

}//namespace Actions

#endif //ACTIONS_H_
