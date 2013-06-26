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
: identifier(), arg(), value(), broadcaster(widget)
{

}
void CommandValue::set(const CommandFlags& flag, const NodeValue& v)
{
    switch(flag)
    {
        case CommandFlags::ACTION:
            action = (MoguSyntax) v;
            break;
        case CommandFlags::OBJECT:
            object = (MoguSyntax) v;
            break;

        case CommandFlags::IDENTIFIER:
            identifier = v;
            break;

        case CommandFlags::ARG:
            arg = v;
            break;

        case CommandFlags::VALUE:
            value = v;
            break;

        case CommandFlags::R_OBJECT:
            r_object = (MoguSyntax) v;
            break;

        case CommandFlags::R_IDENTIFIER:
            r_identifier = v;
            break;

        case CommandFlags::R_ARG:
            r_arg = v;
            break;
        default: return; 
    };
    flags |= (uint8_t) flag;
}

void CommandValue::set(const CommandFlags& flag, MoguSyntax& v)
{
    switch(flag)
    {
        case CommandFlags::OBJECT:
            object = v;
            break;
        case CommandFlags::R_OBJECT:
            r_object = v;
            break;
        case CommandFlags::ACTION:
            action = v;
            break;
        default: break;
    };
    flags |= (uint8_t)flag;
}

NodeValue CommandValue::get(CommandFlags& flag) const
{
    switch(flag)
    {
        case CommandFlags::ACTION:
            return NodeValue((int) action);

        case CommandFlags::OBJECT:
            return NodeValue(int(object));

        case CommandFlags::IDENTIFIER:
            return identifier;

        case CommandFlags::ARG:
            return arg;

        case CommandFlags::VALUE:
            return value;

        case CommandFlags::R_OBJECT:
            return NodeValue((int)r_object);

        case CommandFlags::R_IDENTIFIER:
            return r_identifier;

        case CommandFlags::R_ARG:
            return r_arg;
        
        default: return NodeValue();
    };
}
