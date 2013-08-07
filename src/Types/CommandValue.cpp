/*
 * Command_Value.cpp
 *
 *  Created on: Apr 19, 2013
 *      Author: tom
 */

#include "Command_Value.h"
#include <Moldable/Moldable.h>
#include <Types/Node_Value.h>
#include <Redis/NodeEditor.h>

Command_Value::Command_Value(Moldable& widget)
    : broadcaster(widget)
{}

uint8_t Command_Value::set(const Command_Flags& flag, Node_Value v)
{
    switch(flag)
    {
        case Command_Flags::action:
            action = v;
            break;
        case Command_Flags::object:
            object = Mogu_Syntax::get(v);
            break;
        case Command_Flags::identifier:
            identifier = v;
            break;
        case Command_Flags::arg:
            arg = v;
            break;
        case Command_Flags::value:
            value = v;
            break;
        case Command_Flags::r_object:
            r_object = v;
            break;
        case Command_Flags::r_identifier:
            r_identifier = v;
            break;
        case Command_Flags::r_arg:
            r_arg = v;
            break;
        default: return flags; 
    };
    flags |= (uint8_t) flag;
    return flags;
}

uint8_t Command_Value::set(const Command_Flags& flag, const Syntax_Def& v)
{
    switch(flag)
    {
        case Command_Flags::object:
            object = v;
            break;
        case Command_Flags::r_object:
            r_object = v;
            break;
        case Command_Flags::action:
            action = v;
            break;
        default: break;
    };
    flags |= (uint8_t)flag;
    return flags;
}

Node_Value Command_Value::get(Command_Flags flag) const
{
    switch(flag)
    {
        case Command_Flags::action:
            return Node_Value((int) action);

        case Command_Flags::object:
            return Node_Value(int(object));

        case Command_Flags::identifier:
            return identifier;

        case Command_Flags::arg:
            return arg;

        case Command_Flags::value:
            return value;

        case Command_Flags::r_object:
            return Node_Value((int)r_object);

        case Command_Flags::r_identifier:
            return r_identifier;

        case Command_Flags::r_arg:
            return r_arg;
        
        default: return Node_Value();
    };
}

bool Command_Value::object_is_reduceable(bool r)
{
    Node_Value local_obj {(int) object};

    Command_Flags arg_flag {Command_Flags::arg};
    Command_Flags id_flag {Command_Flags::identifier};
    Command_Flags obj_flag {Command_Flags::object};

    Node_Value& local_arg = r ? r_arg : arg;
    Node_Value& local_id = r ? r_identifier : identifier;

    if (r)
    {
        local_obj.set_int((int) r_object);
        arg_flag = Command_Flags::r_arg;
        id_flag = Command_Flags::r_identifier;
        obj_flag = r_object;
    }

    if (!test(obj_flag)) return false;
    if (!test(id_flag) 
            && (Mogu_Syntax::own != Mogu_Syntax::get(local_arg)))
        return false;
    bool has_arg = test(arg_flag);

    switch(Mogu_Syntax::get(local_obj))
    {
        case Mogu_Syntax::own:
        case Mogu_Syntax::widget:
            return has_arg;
        case Mogu_Syntax::data:
        case Mogu_Syntax::meta:
        case Mogu_Syntax::user:
        case Mogu_Syntax::group:
            break;
        default: return false;
    }
    // If we've reached this point, we have to determine whether
    // or not the node in question is a string, list, or hash.
   
    Prefix p {syntax_to_prefix.at(Mogu_Syntax::get(local_obj))};
    
    Redis::Node_Edtir node {p, (std::string) local_id}
    
    const Syntax_Def& node_type = node.get_type();

    if (!has_arg)
        return node_type == Mogu_Syntax::string;

    return true;

}

std::string Command_Value::join_state(bool r)
{
    Node_Value local_obj {(int) object};
    Command_Flags arg_flag {Command_Flags::arg};
    Command_Flags obj_flag {Commad_Flags::object};
    Command_Flags id_flag {Command_Flags::identifier};
    Node_Value& local_arg = r ? r_arg : arg;
    Node_Value& local_id = r ? r_identifier : identifier;

    if (r)
    {
        local_obj.set_int((int) r_object);
        arg_flag = r_arg;
        obj_flag = r_object;
        id_flag = r_identifier;
    }

    if (!test(obj_flag)) return "";
    
    std::stringstream buf;
    buf << (int) local_obj;

    if (test(id_flag))
        buf << " " << (std::string) local_id;

    if (test(arg_flag))
    {
        buf << " ";
        if (local_arg.is_string()) buf << local_arg.get_string();
        else if (local_arg.is_int()) buf << local_arg.get_int();
        else buf << local_arg.get_float();
    }

    return buf.str();
}
