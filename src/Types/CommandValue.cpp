/*
 * CommandValue.cpp
 *
 *  Created on: Apr 19, 2013
 *      Author: tom
 */

#include "CommandValue.h"
#include <Moldable/Moldable.h>
#include <Types/NodeValue.h>
#include <Redis/NodeEditor.h>

CommandValue::CommandValue(Moldable& widget)
: identifier(), arg(), value(), broadcaster(widget)
{

}
uint8_t CommandValue::set(const CommandFlags& flag, NodeValue v)
{
    switch(flag)
    {
        case CommandFlags::ACTION:
            action = v;
            break;
        case CommandFlags::OBJECT:
            object = MoguSyntax::get(v);
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
            r_object = v;
            break;

        case CommandFlags::R_IDENTIFIER:
            r_identifier = v;
            break;

        case CommandFlags::R_ARG:
            r_arg = v;
            break;
        default: return flags; 
    };
    flags |= (uint8_t) flag;
    return flags;
}

uint8_t CommandValue::set(const CommandFlags& flag, const SyntaxDef& v)
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
    return flags;
}

NodeValue CommandValue::get(CommandFlags flag) const
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

bool CommandValue::objectIsReduceable(bool is_r_object)
{
    NodeValue local_obj((int) object);
    if (is_r_object) local_obj.setInt((int) r_object);
    CommandFlags arg_flag   =
        is_r_object ? CommandFlags::R_ARG : CommandFlags::ARG;
    CommandFlags id_flag    =
        is_r_object ? CommandFlags::R_IDENTIFIER : CommandFlags::IDENTIFIER;
    CommandFlags obj_flag   =
        is_r_object ? CommandFlags::R_OBJECT : CommandFlags::OBJECT;
    NodeValue& local_arg =
        is_r_object ? r_arg : arg;
    NodeValue& local_id  =
        is_r_object ? r_identifier : identifier;

    if (!test(obj_flag)) return false;
    // The only valid object flag that doesn't require an
    // identifier is 'own'
    if (!test(id_flag) &&
            MoguSyntax::own != MoguSyntax::get(local_arg))
        return false;
    bool hasArg = test(arg_flag);
    switch(MoguSyntax::get(local_obj))
    {
        case MoguSyntax::widget:
        case MoguSyntax::own:
            if (!hasArg) return false;
            break;
        // If we have a database field, we'll need to see 
        // whether that field requires an arg.
        case MoguSyntax::data:
        case MoguSyntax::meta:
        case MoguSyntax::user:
        case MoguSyntax::group:
            break;
        default: return false;
    }
    // If we've reached this point, we have to determine whether
    // or not the node in question is a string, list, or hash.
    Redis::NodeEditor node(MoguSyntax::get(local_obj), (std::string) local_id);
    const SyntaxDef& node_type = node.getType();
    if (!hasArg)
        return node_type == MoguSyntax::string;
    else
        return true;
}
std::string CommandValue::stitchState(bool is_r_object)
{
    NodeValue local_obj((int) object);
        if (is_r_object) local_obj.setInt((int) r_object);
        CommandFlags arg_flag   =
            is_r_object ? CommandFlags::R_ARG : CommandFlags::ARG;
        CommandFlags id_flag    =
            is_r_object ? CommandFlags::R_IDENTIFIER : CommandFlags::IDENTIFIER;
        CommandFlags obj_flag   =
            is_r_object ? CommandFlags::R_OBJECT : CommandFlags::OBJECT;
        NodeValue& local_arg =
            is_r_object ? r_arg : arg;
        NodeValue& local_id  =
            is_r_object ? r_identifier : identifier;
    std::string str;

    if (!test(obj_flag)) return EMPTY;
    str += std::to_string((int) local_obj);

    if (test(id_flag))
    {
        str += " ";
        str += (std::string) local_id;
    }
        
    if (test(arg_flag))
    {
        str += " ";
        if (local_arg.isString()) str += (std::string) local_arg;
        else str += std::to_string( (int) local_arg);
    }
    return str;
}
