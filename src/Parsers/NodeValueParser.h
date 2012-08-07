/*
 * NodeValueParser.h
 *
 *  Created on: Jul 11, 2012
 *      Author: tom
 */

#ifndef NODEVALUEPARSER_H_
#define NODEVALUEPARSER_H_

#include <declarations.h>

namespace Parsers
{


class NodeValueParser
{
    Nodes::NodeValue* parsedValue;
    Enums::NodeValueTypes::NodeValueTypes __type;
    std::string __value;
    char __first_char;
    Goo::Moldable* __broadcaster;
    
    std::string polish(const std::string& value);

    void cast_as_enum_repr(const std::string& value);
    void cast_as_enum_int(const std::string& value);
    void interpret_as_field(const std::string& value);
    void interpret_as_integer(const std::string& value);
    void interpret_as_enum(
            const std::string& value, int(*callback)(const std::string&));
    void interpret_as_static_node(const std::string& value);
    void interpret_as_dynamic_node(const std::string& value);
    void interpret_as_float (const std::string& value);
    void interpret_from_file (const std::string& value);
    void set_value_type(
            const std::string& value, int(*callback)(const std::string&) =0);
    void interpret(
            const std::string& value, int(*callback)(const std::string&) =0);

public:
    NodeValueParser(
            std::string value,
            Goo::Moldable* broadcaster =0,
            int(*callback)(const std::string&) =0);
    ~NodeValueParser();
    Nodes::NodeValue* getValue();
};

} // Namespace parsers

#endif /* NODEVALUEPARSER_H_ */
