/*
 * Actions.h
 *
 *  Created on: Apr 22, 2013
 *      Author: tom
 */
#ifndef ACTIONS_H_
#define ACTIONS_H_
#include <declarations.h>
namespace Actions
{

MoguSyntax getPolicyNodeType(const std::string& identifier);
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

#endif //ACTIONS_H_
