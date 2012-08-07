/*
 * NodeValueParser.cpp
 *
 *  Created on: Jul 11, 2012
 *      Author: tom
 */

#include <Parsers/NodeValueParser.h>
#include <Parsers/Parsers.h>
#include <Types/NodeValue.h>
#include <Core/Moldable.h>
#include <Redis/RedisCore.h>
#include <TurnLeftLib/Utils/explosion.h>
#include <fstream>

namespace Parsers
{
using std::string;
using Goo::Moldable;
using std::ifstream;
using namespace Enums::NodeValueTypes;

NodeValueParser::NodeValueParser(string nodeValue, Moldable* broadcaster,
        int(*callback)(const string&), bool value_persists)
{
    parsedValue = new Nodes::NodeValue();
    __first_char = -1;
    __broadcaster = broadcaster;
    set_value_type(nodeValue, callback);
    __value_persists = value_persists;
}

inline string NodeValueParser::polish(const string& value)
{
    return value.substr(1, (value.length()-2));
}

void NodeValueParser::set_value_type(
        const string& value, int(*callback)(const string&))
{
    // Just in case we're dealing with an unhappy case.
    if (__first_char == -1)
    {
        __first_char = value.at(0);
    }

    /* The polished value will be the 'nugget' at the center of
     * all the interpretive wrappers.
     */
    string polished_value;

    if (!isalnum(__first_char))
    {
        /* Make sure we actually NEED to polish this value. First, test
         * to see if it's a cast declaration, which only has one value
         * at the beginning.
         */
        if (__first_char != ENUM_INT_CAST &&
            __first_char != ENUM_REPR_CAST)
        {
        	switch(__first_char)
			{
			case '[':
			case '^':
			case '{':
			case '%':
			case '@':
			case '`':
			case '~':
				polished_value = polish(value);
				break;
			default:
				polished_value = value;
			}
        }
        else
        {
            /* If not, then it's a wrapper declaration, which has one at
             * the beginning, and one at the end.
             */

        		polished_value = value.substr(1,value.length()-1);

        }
    }
    /* If we already have a polished value, just copy it. */
    else
    {
        polished_value = value;
    }


    switch (__first_char)
    {
    case ENUM_REPR_CAST:
        cast_as_enum_repr(value);
        break;
    case ENUM_INT_CAST:
        cast_as_enum_int(value);
        break;
    case '[':
        __type = field_value;
        break;
    case '^':
        __type = integer_value;
        break;
    case '{':
        __type = enum_value;
        break;
    case '<':
        __type = static_node_value;
        break;
    case '@':
        __type = dynamic_node_value;
        break;
    case '~':
        __type = float_value;
        break;
    case '`':
        __type = file_value;
    default:
        __type = string_value;
    }

    interpret(polished_value, callback);
}

void NodeValueParser::interpret(
        const string& value, int(*callback)(const string&))
{
    switch(__type)
    {
    case field_value:
        interpret_as_field(value);
        break;
    case integer_value:
        interpret_as_integer(value);
        break;
    case enum_value:
        interpret_as_enum(value,callback);
        break;
    case static_node_value:
        interpret_as_static_node(value);
        break;
    case dynamic_node_value:
        interpret_as_dynamic_node(value);
        break;
    case float_value:
        interpret_as_float(value);
        break;
    case string_value:
        parsedValue->setString(value);
        break;
    case file_value:
        interpret_from_file(value);
    }
}

void NodeValueParser::cast_as_enum_repr(const string& value)
{
    unsigned int delim = value.find(' ');
    string enum_repr = value.substr(1, delim-1);
    NodeValueTypeParser parser;
    __type = parser.parse(enum_repr);
}

void NodeValueParser::cast_as_enum_int(const string& value)
{
    unsigned int delim = value.find(' ');
    string enum_int_str = value.substr(1, delim-1);
    const char* enum_int_c_str = enum_int_str.c_str();
    char enum_int = (char) atoi(enum_int_c_str);
    __type = (NodeValueTypes) enum_int;
}

void NodeValueParser::interpret_as_field(const string& field)
{
    if (__broadcaster == 0)
    {
        //@TODO Throw an error!
    }
    string nodeName = __broadcaster->getNodeList()->at(0);
    string command = "hget ";
    command.append(nodeName).append(" ").append(field);
    Redis::command(command);
    string reply_str = Redis::toString();
    parsedValue->setString(reply_str);
}

inline void NodeValueParser::interpret_as_integer(const string& value)
{
    const char* value_c_str = value.c_str();
    parsedValue->setInt(atoi(value_c_str));
}

inline void NodeValueParser::interpret_as_enum(
        const string& value, int(*callback)(const string&))
{

	std::string masks[8];
	// Look for special properties in the widget and turn on the
	// corresponding bits.
	TurnLeft::Utils::Explosion explosion(value);
	explosion.explode('|',masks);
	int e_val = callback(masks[0]);

	int num_words = explosion.getNumWords();
	if (num_words > 1)
	{
		Parsers::BitMaskParser maskParser;
		for (int w = 1; w < num_words; w++)
		{
			e_val |= maskParser.parse(masks[w]);
		}
	}

    parsedValue->setInt( e_val );
}

void NodeValueParser::interpret_as_static_node(const string& value)
{
    int delim = value.find(' ');
    if (!delim == -1)
    {
        string command = "get ";
        command.append(value);
        Redis::command(command);
        string reply_str = Redis::toString();
        parsedValue->setString(reply_str);
    }
    else
    {
        char nextChar = value.at(delim+1);
        int num_chars = value.length() - (delim+1);
        string arg = value.substr(delim+1, num_chars);
        string nodeName = value.substr(0,delim);
        string command;
        if (isalpha(nextChar))
        {
            string command = "hget ";
            command.append(nodeName).append(" ").append(arg);
            Redis::command(command);
            string reply_str = Redis::toString();
            parsedValue->setString(reply_str);
        }
        else
        {
            command = "lindex ";
            command.append(nodeName).append(" ").append(arg);
            Redis::command(command);
            string reply_str = Redis::toString();
            parsedValue->setString(reply_str);
        }
    }
}

void NodeValueParser::interpret_as_dynamic_node(const string& value)
{
    //@TODO Figure this out
}

void NodeValueParser::interpret_as_float(const string& value)
{
    const char* value_c_str = value.c_str();
    parsedValue->setFloat(atof(value_c_str));
}

void NodeValueParser::interpret_from_file(const string& value)
{
    const char* fileName = value.c_str();
    ifstream buffer;
    buffer.open(fileName);
    std::string contents;
    if (buffer.is_open())
    {
        while (!buffer.eof())
        {
            if (buffer.eof()) break;
            buffer >> contents;
        }
    }
    parsedValue->setString(contents);
}

Nodes::NodeValue* NodeValueParser::getValue()
{
    return parsedValue;
}

NodeValueParser::~NodeValueParser()
{
    // Do not delete parsedValue if it's being  used elsewhere.
	if (!__value_persists)
	{
		delete parsedValue;
	}
}

} // namespace Parsers
