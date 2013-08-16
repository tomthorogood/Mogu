/*
 * Command_Value.h
 *
 *  Created on: Apr 19, 2013
 *      Author: tom
 */

#ifndef COMMANDVALUE_H_
#define COMMANDVALUE_H_

#include <Types/NodeValue.h>
#include <Parsers/NodeValueParser.h>
/*!\brief Holds parsed values from the CommandParser which can then be used
 * to handle events.
 */

enum class Command_Flags {
    action          =1
    , object        =2
    , identifier    =4
    , arg           =8
    , value         =16
    , r_object      =32
    , r_identifier  =64
    , r_arg         =128
};

class Command_Value
{
public:
    Command_Value(Moldable& widget);

    uint8_t set(const Command_Flags& flag, Node_Value v);
    uint8_t set(const Command_Flags& flag, const Syntax_Def& v);
    Node_Value& get ( Command_Flags flag);

    inline const std::string& get_identifier() const
        { return identifier.get_string(); }
    
    inline const uint8_t& get_flags() const 
        { return flags; }
    
    inline Moldable& get_widget()
        { return broadcaster; }
  
    std::string join_state(bool reverse=false);  
    bool object_is_reduceable(bool reverse_object=false);

    inline bool is_reversed() const { return reversed; }

    inline bool test (Command_Flags flag) const
        { return flags & (uint8_t) flag; }

    void reduce_object (bool reverse=false, Moldable* broadcaster =nullptr)
    {
        Parsers::Node_Value_Parser nvp {};
        nvp.give_input(join_state(reverse), value);
        set(Command_Flags::value, value);

        Command_Flags arg_flag = reverse ? Command_Flags::r_arg
            : Command_Flags::arg;
        Command_Flags obj_flag = reverse ? Command_Flags::r_object
            : Command_Flags::object;
        Command_Flags id_flag  = reverse ? Command_Flags::r_identifier
            : Command_Flags::identifier;

        if (test(arg_flag)) flags -= (uint8_t) arg_flag;
        if (test(obj_flag)) flags -= (uint8_t) obj_flag;
        if (test(id_flag)) flags -= (uint8_t) id_flag;
    }

private:
    bool reversed {false};
    
    Node_Value action {};
    Node_Value object {};
    Node_Value r_object {};

    Node_Value identifier {};
    Node_Value r_identifier {};
    
    Node_Value arg {};
    Node_Value r_arg {};

    Node_Value value {};

    Moldable& broadcaster;

    uint8_t flags {};
};


#endif /* COMMANDVALUE_H_ */
