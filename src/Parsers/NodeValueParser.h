/*
 * Node_Value_Parser.h
 *
 *  Created on: March 31, 2013
 *      Author: cameron
 */

#ifndef NODEVALUEPARSER_H_
#define NODEVALUEPARSER_H_

#include "Node_Value_Parser/State_Parser.h"
#include "Node_Value_Parser/Math_Parser.h"
#include "Node_Value_Parser/Token_Manager.h"

class NodeValue;
class command_value;

#include <string>
#include <vector>

namespace Parsers {

class Node_Value_Parser 
{
	public:
		Node_Value_Parser() : state_parser(tm), math_parser(tm) {}
        void give_input(std::string input, NodeValue& output,
                const SyntaxDef& context, NodeValue* arg=nullptr);
		void give_input(const std::string& input, NodeValue& nv,
		    Moldable* bc = nullptr);
		void give_input(const std::string& input, command_value& cv,
		    Moldable *bc = nullptr);

	private:
		Token_Manager tm {};
        std::string input {};
		State_Parser stateParser;
		Math_Parser mathParser;

		void tokenize_input(std::string input, bool setAtBeginning=false);
        void hash_next_token();
		void reduce_expressions(Moldable* bc);
		int find_full_quote(std::string str);
        void handle_append_command(command_value&, Moldable*);
        void set_command_value_object(command_value&, bool r_object=false); 
};

}	// namespace Parsers

#endif /* NODEVALUEPARSER_H_ */
