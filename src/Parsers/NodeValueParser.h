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
#include <string>
#include <vector>

class Node_Value;
class Command_Value;


namespace Parsers {

class Node_Value_Parser 
{
	public:
		Node_Value_Parser() : state_parser(tm), math_parser(tm) {}
        void give_input(std::string input, Node_Value& output,
                const Syntax_Def& context, Node_Value* arg=nullptr);
		void give_input(const std::string& input, Node_Value&,
		    Moldable* bc = nullptr);
		void give_input(const std::string& input, Command_Value&,
		    Moldable *bc = nullptr);

	private:
		Token_Manager tm {};
        std::string input {};
		State_Parser state_parser;
		Math_Parser math_parser;

		void tokenize_input(std::string input, bool set_at_beginning=false);
        void hash_next_token();
		void reduce_expressions(Moldable* bc);
		int find_full_quote(std::string str);
        void handle_append_command(command_value&, Moldable*);
        void set_command_value_object(command_value&, bool r_object=false); 
};

}	// namespace Parsers

#endif /* NODEVALUEPARSER_H_ */
