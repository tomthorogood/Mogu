/*
 * Actions.h
 *
 *  Created on: Apr 22, 2013
 *      Author: tom
 */

#include <declarations.h>
namespace Actions
{

void set(Moldable& broadcaster, CommandValue& command);
void increment(Moldable& broadcaster, CommandValue& command);
void decrement(Moldable& broadcaster, CommandValue& command);
void test(Moldable& broadcaster, CommandValue& command);
void email(Moldable& broadcaster, CommandValue& command);
void reload(Moldable& broadcaster, CommandValue& command);
void append(Moldable& broadcaster, CommandValue& command);
void reset(Moldable& broadcaster, CommandValue& command);
void remove(Moldable& broadcaster, CommandValue& command);
void javascript(Moldable& broadcaster, CommandValue& command);

}//namespace Actions

