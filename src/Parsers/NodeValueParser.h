/*
 * Node_Value_Parser.h
 *
 *  Created on: March 31, 2013
 *      Author: cameron
 */

#ifndef NODEVALUEPARSER_H_
#define NODEVALUEPARSER_H_

#include "NodeValueParser/StateParser.h"
#include "NodeValueParser/MathParser.h"
#include "NodeValueParser/TokenManager.h"
#include <string>
#include <vector>

class Node_Value;
class Command_Value;


namespace Parsers {

class Node_Value_Parser 
{
	public:
		Node_Value_Parser() : state_parser(tm), math_parser(tm) {}
		void give_input(const std::string& input, Node_Value&,
		    Moldable* bc = nullptr);
		void give_input(const std::string& input, Command_Value&,
		    Moldable *bc = nullptr);

        inline void set_user_id(const int& i) { user_id = i; }
        inline void set_group_id(const int& i) { group_id = i; }
        inline bool resolved_dynamic_value() const { return resolved_dynamic; }

	private:
		Token_Manager tm {};
        std::string input {};
		State_Parser state_parser;
		Math_Parser math_parser;
        bool resolved_dynamic {true};
        int group_id {};
        int user_id {};

		void tokenize_input(std::string input, bool set_at_beginning=false);
        void hash_next_token();
		void reduce_expressions(Moldable* bc);
		size_t find_full_quote(std::string str);
        void handle_append_command(Command_Value&, Moldable*);
        void set_command_value_object(Command_Value&, bool r_object=false); 
};

}	// namespace Parsers

#endif /* NODEVALUEPARSER_H_ */
