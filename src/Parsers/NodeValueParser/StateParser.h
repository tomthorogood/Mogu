/*
 * State_Parser.h
 *
 *  Created on: March 31, 2013
 *      Author: cameron
 */

#ifndef STATEPARSER_H_
#define STATEPARSER_H_

#include <string>
#include "../../Moldable/Moldable.h"
#include "TokenManager.h"

namespace Parsers {

class State_Parser
{
	public:
		State_Parser(Token_Manager& tm_) : tm(tm_) {}
		void process_input(Moldable* broadcaster);

	private:
		Token_Manager& tm;

        // When a TOKEN_DELIM is expected as a placeholder for an
        // object's identifier, this method will advance the TM pointer,
        // assure that the next token is a TOKEN_DELIM, advance the TM pointer
        // to one step after the TOKEN_DELIM, and return the resolved identifier.
        // Used as a function since it's a common operation.
        std::string get_identifier();

        // Looks up the state of a named widget
        void handle_widget(
            const std::string& identifier, Node_Value& result); 

        // Looks up the state of a self-referential widget "own|self"
        void handle_widget(
            Moldable* broadcaster, Node_Value& result);

        // Looks up the value of a static data field in Redis.
        void handle_data(
            const std::string& identifier, Node_Value& result);

        // Looks up the value of a user's data field in Redis.
        void handle_user_field(
            const std::string& identifier, Node_Value& result);

        // Looks up the value of a group's data field in Redis
        void handle_group_field (
            const std::string& identifier, Node_Value& result);

};

}	// namespace Parsers
#endif /* STATEPARSER_H_ */
