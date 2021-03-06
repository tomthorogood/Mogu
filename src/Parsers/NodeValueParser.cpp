/*
 * Node_Value_Parser.cpp
 *
 *  Created on: March 31, 2013
 *      Author: cameron
 */
#include "NodeValueParser.h"
#include <Types/NodeValue.h>
#include <Types/syntax.h>
#include "../Types/CommandValue.h"
#include <hash.h>
#include <cctype>
#include "../Config/inline_utils.h"

//debug
#include <iostream>

namespace Parsers {

/*!\brief When given a string where a second quote is expected, extracts
 * the full string, even if there are internal quotes. The opening quote does
 * not need to be passed in. If there is more than one quoted string present,
 * only extracts the first one:
 *
 * "Hello, my name is Tom \"Mountain\" Murphy," said the man, "I eat figs."
 * Would return the index of the last character in:
 * "Hello, my name is Tom \"Mountain\" Murphy,"
 *
 * !\return the index of the final quotation mark.
 */

size_t Node_Value_Parser::find_full_quote(std::string s)
{
    // Trim the leading quote.
    if (s.at(0) == '"')
        s = s.substr(1);

    size_t q {s.find_first_of('"')};
    if (q == std::string::npos) return 0;
    /* Determine whether the character preceeding the quotation mark is
     * an escape delimiting slash.
     */
    while (s.at(q-1) == '\\')
    {
        q = s.find('"',q+1);
    };
    return q;
}
void Node_Value_Parser::tokenize_input(std::string in, bool set_at_begin)
{
    size_t input_index {};
    size_t end_index {};
    size_t input_size {in.size()};

    while (input_index < input_size)
    {
        if (in[input_index] == '"')
        {
            size_t q_index {find_full_quote(in.substr(input_index+1))};
            end_index = q_index + input_index +1;
        }
        else
        {
            size_t search {input.find(' ',input_index)};
            if (search == std::string::npos)
                end_index = in.size();
            else
                end_index = search -1;
        }

        size_t substr_len {end_index-input_index+1};
        if (substr_len > in.size()) substr_len = in.size();
        std::string t {in.substr(input_index, substr_len)};
        if (!t.empty())
        {
            if (isdigit(t[0]))
                tm.add_token(std::stoi(t));
            else
                tm.add_token(t);
        }
        input_index = end_index +2;
    }

    if (set_at_begin)
        tm.begin();
    else
        tm.end();
}

/* Iterates through a vector of tokens and applies the core NvP logic
 * to reduce them down to one expression.
 */
void Node_Value_Parser::reduce_expressions(Moldable* bc)
{
    // If there is only one token, it cannot be reduced any further.
	if (tm.size() == 1)
	{
	    tm.begin();
	    return;
	}
	
    int curr_token {tm.current_token()};

    
	/* Iterate backwards through the vector of tokens until the
	 * 'Begin' token is reached.
	 */
	while(curr_token != Mogu_Syntax::OUT_OF_RANGE_BEGIN)
	{
	    /* The '(' syntax will always signify a mathematical expression.
	     * Hand this job over to the MathParser.
	     */
		if(curr_token == Mogu_Syntax::OPER_OPPAREN) 
			math_parser.process_input();
			
		/*  Object tokens will refer to a database field or widget state
		 *  that will be reduced to a string or integer.
		 */
		else if(is_object_token(curr_token))
        {
            state_parser.set_user_id(user_id);
            state_parser.set_group_id(group_id);
            state_parser.process_input(bc);
        }

		tm.prev();
		curr_token = tm.current_token();
	}
    tm.begin();
    if (tm.current_token()==Mogu_Syntax::hash)
    {
        hash_next_token();
        tm.begin();
    }
}


void Node_Value_Parser::hash_next_token()
{
    assert(Mogu_Syntax::hash.integer == tm.current_token());
    std::string hashed_value {};
    tm.save_location();
    tm.next();
    if (tm.current_token() == Mogu_Syntax::TOKEN_DELIM)
        hashed_value = Hash::to_hash(tm.fetch_string());
    else
        hashed_value = Hash::to_hash(std::to_string(tm.current_token()));
    tm.delete_from_saved();
    tm.inject_token(hashed_value);
}

void Node_Value_Parser::give_input(const std::string& i, Node_Value& nv, Moldable* bc)
{
    input = i;
    if (input.empty())
    {
        nv.set_string("");
        return;
    }
	tokenize_input(input);
	reduce_expressions(bc);

    /* As long as we have input that is reducable, keep reducing it.*/
	while (Mogu_Syntax::TOKEN_DELIM == tm.current_token()
	    && (!tm.fetch_string().empty())
	    && (tm.fetch_string().at(0)!='"')
	    && (tm.fetch_string().find_first_of(" ") != std::string::npos))
	{
	    std::string new_input = tm.fetch_string();

	    if (input!=new_input)
	    {
	        tm.reset();
	        input = new_input;
	        tokenize_input(input);
            reduce_expressions(bc);
	    }
	    else break;
	}

	//if we have more than one token in numTokens at this point (or
	//two if the first token is TOKEN DELIM), something has gone wrong

	if(tm.current_token() == Mogu_Syntax::TOKEN_DELIM)
		nv.set_string(tm.fetch_string());
	else
		nv.set_int((int)tm.current_token());
	tm.reset(); // Clear the TokenManager to prepare for the next input

}

void Node_Value_Parser::set_command_value_object(Command_Value& cv, bool r_tokens)
{
    int curr_tok {};
    Node_Value tmp {};

    Command_Flags obj_flag {Command_Flags::object};
    Command_Flags id_flag {Command_Flags::identifier};
    Command_Flags arg_flag {Command_Flags::arg};

    if (r_tokens)
    {
        obj_flag = Command_Flags::r_object;
        id_flag = Command_Flags::r_identifier;
        arg_flag = Command_Flags::r_arg;
    }

    // Fast forward to the next object token.
    do {
        curr_tok = tm.current_token();
        tm.next();
    } while (!is_object_token(curr_tok));
    tm.prev();

    cv.set(obj_flag, curr_tok);
    tm.next();

    // If the next token is a string, it is necessarily an identifier.
    // It can also be another arbitrary token, however, representing a 
    // state/attribute("own content", or "app path"). In the latter case,
    // we need nothing more, though, so we return.
    curr_tok = tm.current_token();
    if (curr_tok == Mogu_Syntax::TOKEN_DELIM)
    {
        tmp.set_string(tm.fetch_string());
        cv.set(id_flag, tmp);
    }
    else if (curr_tok != Mogu_Syntax::OUT_OF_RANGE_END)
    {
        tmp.set_int(curr_tok);
        cv.set(arg_flag, tmp);
        return;
    }

    // If we've reached this part, we're setting an arg, no ifs, ands, or buts.
    tm.next();
    curr_tok = tm.current_token();
    if (curr_tok == Mogu_Syntax::TOKEN_DELIM)
        tmp.set_string(tm.fetch_string());
    else
        tmp.set_int(curr_tok);
    cv.set(arg_flag, tmp);
}

void Node_Value_Parser::handle_user_to_group(Command_Value& cv, Moldable* bc)
{
    if (tm.current_token() != Mogu_Syntax::preposition) return;
    tm.next();
    if (tm.current_token() != Mogu_Syntax::group) return;
    tm.next();

    cv.set(Command_Flags::object, Mogu_Syntax::group);

    std::stringstream buf;
    while (tm.current_token() != Mogu_Syntax::OUT_OF_RANGE_END)
    {
        if (tm.current_token() == Mogu_Syntax::TOKEN_DELIM)
            buf << tm.fetch_string();
        else buf << tm.current_token();
        buf << " ";
        tm.next();
    }
    Node_Value_Parser p {};
    Node_Value r {};
    p.give_input(buf.str(),r,bc);
    cv.set(Command_Flags::identifier, r);
}

void Node_Value_Parser::handle_append_command(Command_Value& cv, Moldable* bc)
{
    if (tm.current_token() != Mogu_Syntax::append) return;
    
    Node_Value tmp {};
    std::string str {};
    uint8_t flags {cv.get_flags()};
    int token {tm.current_token()};
    bool preposition_found {false};

    // This is used for checking unexpected input to see if it's actually
    // supposed to be represented as an integer used as a list index.
    bool check_if_list {false};

    tm.next();
    token = tm.current_token();

    // Cycle through the input, testing flag combinations and setting 
    // things where appropriate, until we're out of tokens.
    while (token != Mogu_Syntax::OUT_OF_RANGE_END)
    {
        // A string will either be a value or an identifier.
        if (!preposition_found && Mogu_Syntax::location == token)
        {
            tm.next();
            std::stringstream buf;
            // append location user foo bar to self
            while (token != Mogu_Syntax::preposition)
            {
                if (Mogu_Syntax::TOKEN_DELIM == token)
                    buf << tm.fetch_string();
                else buf << tm.current_token();
                buf << " ";
                tm.next();
                token = tm.current_token();
            }
            std::string in {buf.str()}; //user foo bar
            Node_Value_Parser p {};
            Command_Value c {*bc};
            Node_Value v {};
            p.set_user_id(user_id);
            p.set_group_id(group_id);
            p.give_input(in,v); //"widget baz bop"
            in = stripquotes(v.get_string()); // widget baz bop
            buf.str(std::string{});
            buf << Mogu_Syntax::append.str << " " << in << " ";
            while (token != Mogu_Syntax::OUT_OF_RANGE_END)
            {
                if (Mogu_Syntax::TOKEN_DELIM == token)
                    buf << tm.fetch_string();
                else buf << tm.current_token();
                buf << " ";
                tm.next();
                token = tm.current_token();
            }
            p.give_input(buf.str(),c,bc);
            cv = c;
            return;
        }
        if (Mogu_Syntax::TOKEN_DELIM == token)
        {
            str = tm.fetch_string();
            tmp.set_string(str);
            // If the only thing that has been set is the action, 
            // then the token delimiter will be a quoted string.
            if ((flags == (uint8_t)Command_Flags::action) && is_quoted_string(str))
            {
                flags = cv.set(Command_Flags::value, tmp);
            }
            // If the r_object has been set, but there's no identifier,
            // the next string encountered must be the identifier.
            else if (cv.test(Command_Flags::r_object) && 
                    !cv.test(Command_Flags::r_identifier))
            {
                flags = cv.set(Command_Flags::r_identifier, tmp);
            }
            // Same as above, but for standard objects.
            else if (cv.test(Command_Flags::object) && 
                    !cv.test(Command_Flags::identifier))
            {
                flags = cv.set(Command_Flags::identifier, tmp);
            }
            // In all other cases, this is unexpected input, but it might
            // be a list index. 'continue' not present here because we want to
            // keep moving through the decision tree.
            else check_if_list = true;
        }
        // Object tokens will either be the object or r_object, depending
        // on if it was reached before the preposition or not.
        else if (is_object_token(token))
        {
            if (!preposition_found && !cv.test(Command_Flags::r_object))
            {
                flags = cv.set(Command_Flags::r_object, token);
                if (token == Mogu_Syntax::user)
                {   // The only instance where we have
                    // "append user to ..." is when we are
                    // appending a user to a group.
                    tm.next();
                    if (tm.current_token()==Mogu_Syntax::preposition)
                    {
                        handle_user_to_group(cv,bc);
                        break;
                    }
                    else
                        tm.prev();
                }
            }
            else if (preposition_found && !cv.test(Command_Flags::object))
            {
                flags = cv.set(Command_Flags::object, token);
            }
            else
                check_if_list = true;
                // Do not continue.
        }
        else if (is_state_token(token))
        {
            tmp.set_int(token);
            if (!preposition_found && !cv.test(Command_Flags::r_arg))
            {
                flags = cv.set(Command_Flags::r_arg, tmp);
            }
            else if (preposition_found && !cv.test(Command_Flags::arg))
            {
                flags = cv.set(Command_Flags::arg, token);
            }
            else
                check_if_list = true;
        }
        else if (is_preposition_token(token))
        {
            preposition_found = true;
        }
        // This would be reached in the event of unexpected input.
        // If the check_if_list flag is turned on, we're going to give the 
        // input the benefit of the doubt and see whether or not the previous
        // token was a list token. If so, that's what this is. Otherwise, 
        // we're going to return without doing anything.
        else if (check_if_list)
        {
            tm.prev();
            if (Mogu_Syntax::list != tm.current_token())
                return;
            tm.next(); // Make sure to go back where we were in the input.
            tmp.set_int((int) token);
            if (!preposition_found && !cv.test(Command_Flags::r_arg)) 
            {
                cv.set(Command_Flags::r_arg, tmp);
            }
            else if (preposition_found && !cv.test(Command_Flags::arg))
            {
                cv.set(Command_Flags::arg, tmp);
            }
            // We have no clue what the hell this is. 
            else return;
        }

        tm.next();
        token = tm.current_token();
    } // end while loop

    // After this, there is the possibility that we'll have a reduceable value
    // with r_OBJ/r_ID/r_arg. If so, we'll go ahead and reduce that now:
    if (cv.object_is_reduceable(true))
    {
        cv.reduce_object(true, bc);
    }
}

void Node_Value_Parser::give_input(const std::string& i, Command_Value& cv,
    Moldable* bc)
{
    Node_Value tmp {};
    input = i;
    tokenize_input(input, true); // Make sure to return the iterator to
                                // the BEGINNING
    // The first token is always an action
    cv.set(Command_Flags::action, tm.current_token());

    if (Mogu_Syntax::append == cv.get(Command_Flags::action))
    {
        handle_append_command(cv,bc);
        tm.reset();
        return;
    }

    // The second token is awlays the start of the object set.
    tm.next();
    int tok = tm.current_token();
    cv.set(Command_Flags::object, tok);
    tm.next();

    while (tok != Mogu_Syntax::OUT_OF_RANGE_END)
    {
        tok = tm.current_token();
        if (tok == Mogu_Syntax::TOKEN_DELIM)
        {
            std::string string_token = tm.fetch_string();
            if (tm.is_quoted_string(string_token)) {
                tm.reset();
                return;
            }
            /* The identifier should always be the first TOKEN_DELIM
             * encountered.
             */
            if (cv.get_identifier().empty())
            {
                Node_Value tmp {};
                tmp.set_string(string_token);
                cv.set(Command_Flags::identifier, tmp);
            }
            else
            {
                /* The only other TOKEN_DELIM that should be encountered before
                 * a preposition should be an argument (ie: database hash field)
                 */
                tmp.set_string(string_token);
                cv.set(Command_Flags::arg, tmp);
            }
        }
        else if (is_state_token(tok))
        {
            tmp.set_int(tok);
            cv.set(Command_Flags::arg, tmp);
        }
        else if (is_preposition_token(tok)) {
            bool reduce {true};
            /* First, check to see if the next value is 'location' */
            tm.next();
            int tok = tm.current_token();

            if (tm.current_token() == Mogu_Syntax::location)
            {
                reduce = false;
                tm.next();
                tok = tm.current_token();
            }

            std::stringstream buf;
            while (tok != Mogu_Syntax::OUT_OF_RANGE_END)
            {
                if (tok == Mogu_Syntax::TOKEN_DELIM)
                    buf << tm.fetch_string();
                else buf << tok;
                buf << " ";
                tm.next();
                tok = tm.current_token();
            }

            std::string s{buf.str()};
            Node_Value r {};

            if (reduce)
            {
                Node_Value_Parser p {};
                p.set_user_id(user_id);
                p.set_group_id(group_id);
                p.give_input(s,r,bc);
            }
            else
            {
                r.set_string(s);
            }
            cv.set(Command_Flags::value, r);
            break; // After parsing a prepositional, there is nothing left to do.
        }
        tm.next();
    }
    tm.reset();
}

}	// namespace Parsers
