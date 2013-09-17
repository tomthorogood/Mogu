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

void set        (Moldable&, Command_Value&);
void increment  (Moldable&, Command_Value&);
void decrement  (Moldable&, Command_Value&);
void test       (Moldable&, Command_Value&);
void email      (Moldable&, Command_Value&);
void reload     (Moldable&, Command_Value&);
void append     (Moldable&, Command_Value&);
void reset      (Moldable&, Command_Value&);
void remove     (Moldable&, Command_Value&);
void javascript (Moldable&, Command_Value&);
void clear      (Moldable&, Command_Value&);
void emit       (Moldable&, Command_Value&);
void center_vertically (Moldable&, Command_Value&);

}//namespace Actions

#endif //ACTIONS_H_
