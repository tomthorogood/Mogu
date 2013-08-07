/*
 * Node_ValueParser.cpp
 *
 *  Created on: March 31, 2013
 *      Author: cameron
 */
#include "Node_ValueParser.h"
#include <Types/Node_Value.h>
#include <Types/syntax.h>
#include <Types/command_value.h>
#include <hash.h>
#include <cctype>

//debug
#include <iostream>

namespace Parsers {

Node_ValueParser::Node_ValueParser() : state_parser(tm), math_parser(tm)
{
}

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

size_t Node_ValueParser::find_full_quote(std::string s)
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
void Node_ValueParser::tokenize_input(std::string in, bool set_at_begin)
{
    size_t input_index {};
    size_t input_size {in.size()};
    size_t end_index {};
    while (end_index < input_size)
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

        std::string t {in.substr(input_index, (end_index-input_index+1))};
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
void Node_ValueParser::reduce_expressions(Moldable* bc)
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
            state_parser.process_input(bc);

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

/*!\brief Allows for contextual interpretation of iterative data such as
 * user field in a list of users,
 * specific item in a list of items
 *
 *!\param input_ The input string
 *!\param output_ The container in which the result will be placed
 *!\param member_context_ The context [user|list|data]
 *!\param arg An optional Node_Value containing an argument to be used with the
 *      context. Default: nullptr
 */
void Node_ValueParser::give_input(
    std::string input_
    , Node_Value& output
    , const Syntax_Def& member_context_
    , Node_Value* arg)
{

    switch(member_context_)
    {
        case Mogu_Syntax::user:
        case Mogu_Syntax::list:
            sreplace(input_, "member", "user");
            break;
        case Mogu_Syntax::data:
            sreplace(input_, "member", "data");
            break;
        default:
            break;
    }

    if ((member_context_ != Mogu_Syntax::__NONE__) && arg)
    {
        input_ += " ";

        if (arg->isString())
            input_ += arg->getString();
        else if (arg->isInt())
            input_ += std::to_string(arg->getInt());
        else
            input_ += std::to_string(arg->getFloat());
    }

    give_input(input_,output);
}

void Node_ValueParser::hash_next_token()
{
    assert(Mogu_Syntax::hash.integer == tm.current_token());
    std::string hashed_value;
    tm.save_location();
    tm.next();
    if (tm.current_token() == Mogu_Syntax::TOKEN_DELIM)
        hashed_value = Hash::toHash(tm.fetch_string());
    else
        hashed_value = Hash::toHash(std::to_string(tm.current_token()));
    tm.deleteFromSaved();
    tm.injectToken(hashed_value);
}

void Node_ValueParser::give_input(const std::string& input_, Node_Value& nv, Moldable* bc)
{
    input = input_;
    if (input.empty())
    {
        nv.set_string("");
        return;
    }
	tokenize_input(input);
	reduce_expressions(bc);

    /* As long as we have input that is reducable, keep reducing it.*/
	while (Mogu_Syntax::TOKEN_DELIM == tm.current_token()
	    && (tm.fetch_string().at(0)!='"')
	    && (tm.fetch_string().find_first_of(" ") != std::string::npos))
	{
	    std::string new_input = tm.fetch_string();
	    tm.reset();
	    tokenize_input(new_input);
	    reduce_expressions(bc);
	}

	//if we have more than one token in numTokens at this point (or
	//two if the first token is TOKEN DELIM), something has gone wrong

	if(tm.current_token() == Mogu_Syntax::TOKEN_DELIM)
		nv.set_string(tm.fetch_string());
	else
		nv.set_int((int)tm.current_token());
	tm.reset(); // Clear the TokenManager to prepare for the next input

}

void Node_ValueParser::set_command_value_object(command_value& cv, bool r_tokens)
{
    int currTok;
    Node_Value tmp;

    Command_Flags obj_flag    = r_tokens ? Command_Flags::R_OBJECT :
        Command_Flags::OBJECT;
    Command_Flags id_flag     = r_tokens ? Command_Flags::R_IDENTIFIER :
        Command_Flags::IDENTIFIER;
    Command_Flags arg_flag    = r_tokens ? Command_Flags::R_ARG :
        Command_Flags::ARG;

    // Fast forward to the next object token.
    do {
        currTok = tm.current_token();
        tm.next();
    } while (!is_object_token(currTok));
    tm.prev();

    cv.set(obj_flag, currTok);
    tm.next();

    // If the next token is a string, it is necessarily an identifier.
    // It can also be another arbitrary token, however, representing a 
    // state/attribute("own content", or "app path"). In the latter case,
    // we need nothing more, though, so we return.
    currTok = tm.current_token();
    if (currTok == Mogu_Syntax::TOKEN_DELIM)
    {
        tmp.set_string(tm.fetch_string());
        cv.set(id_flag, tmp);
    }
    else if (currTok != Mogu_Syntax::OUT_OF_RANGE_END)
    {
        tmp.set_int( (int) currTok );
        cv.set(arg_flag, tmp);
        return;
    }

    // If we've reached this part, we're setting an arg, no ifs, ands, or buts.
    tm.next();
    currTok = tm.current_token();
    if (currTok == Mogu_Syntax::TOKEN_DELIM)
        tmp.set_string(tm.fetch_string());
    else
        tmp.set_int( (int) currTok);
    cv.set(arg_flag, tmp);
}

void Node_ValueParser::handle_append_command(command_value& cv, Moldable* bc)
{
    int token;
    Node_Value tmpValue;
    std::string str;
    uint8_t flags = cv.getFlags();
    bool preposition_found = false;

    // This is used for checking unexpected input to see if it's actually
    // supposed to be represented as an integer used as a list index.
    bool check_if_list;

    if (tm.current_token() != Mogu_Syntax::append) return;
    tm.next();
    token = tm.current_token();

    // Cycle through the input, testing flag combinations and setting 
    // things where appropriate, until we're out of tokens.
    while (token != Mogu_Syntax::OUT_OF_RANGE_END)
    {
        // A string will either be a value or an identifier.
        if (Mogu_Syntax::TOKEN_DELIM == token)
        {
            str = tm.fetch_string();
            tmpValue.set_string(str);
            // If the only thing that has been set is the action, 
            // then the token delimiter will be a quoted string.
            if ((flags == (uint8_t)Command_Flags::ACTION) && is_quoted_string(str))
            {
                flags = cv.set(Command_Flags::VALUE, tmpValue);
            }
            // If the R_OBJECT has been set, but there's no identifier,
            // the next string encountered must be the identifier.
            else if (cv.test(Command_Flags::R_OBJECT) && 
                    !cv.test(Command_Flags::R_IDENTIFIER))
            {
                flags = cv.set(Command_Flags::R_IDENTIFIER, tmpValue);
            }
            // Same as above, but for standard objects.
            else if (cv.test(Command_Flags::OBJECT) && 
                    !cv.test(Command_Flags::IDENTIFIER))
            {
                flags = cv.set(Command_Flags::IDENTIFIER, tmpValue);
            }
            // In all other cases, this is unexpected input, but it might
            // be a list index. 'continue' not present here because we want to
            // keep moving through the decision tree.
            else check_if_list = true;
        }
        // Object tokens will either be the OBJECT or R_OBJECT, depending
        // on if it was reached before the preposition or not.
        else if (is_object_token(token))
        {
            if (!preposition_found && !cv.test(Command_Flags::R_OBJECT))
            {
                flags = cv.set(Command_Flags::R_OBJECT, token);
            }
            else if (preposition_found && !cv.test(Command_Flags::OBJECT))
            {
                flags = cv.set(Command_Flags::OBJECT, token);
            }
            else
                check_if_list = true;
                // Do not continue.
        }
        else if (is_state_token(token))
        {
            tmpValue.set_int((int) token);
            if (!preposition_found && !cv.test(Command_Flags::R_ARG))
            {
                flags = cv.set(Command_Flags::R_ARG, tmpValue);
            }
            else if (preposition_found && !cv.test(Command_Flags::ARG))
            {
                flags = cv.set(Command_Flags::ARG, token);
            }
            else
                check_if_list = true;
                // Do not continue;
        }
        else if (isPrepositionToken(token))
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
            tmpValue.set_int((int) token);
            if (!preposition_found && !cv.test(Command_Flags::R_ARG)) 
            {
                cv.set(Command_Flags::R_ARG, tmpValue);
            }
            else if (preposition_found && !cv.test(Command_Flags::ARG))
            {
                cv.set(Command_Flags::ARG, tmpValue);
            }
            // We have no clue what the hell this is. 
            else return;
        }

        tm.next();
        token = tm.current_token();
    } // end while loop

    // After this, there is the possibility that we'll have a reduceable value
    // with R_OBJ/R_ID/R_ARG. If so, we'll go ahead and reduce that now:
    if (cv.objectIsReduceable(true))
    {
        cv.reduceObject(true, bc);
    }
       
}

void Node_ValueParser::give_input(const std::string& input_, command_value& cv,
    Moldable* bc)
{
    Node_Value tmp;
    input = input_;
    tokenize_input(input, true); // Make sure to return the iterator to
                                // the BEGINNING
    // The first token is always an action
    cv.set(Command_Flags::ACTION, tm.current_token());

    if (Mogu_Syntax::append == cv.get(Command_Flags::ACTION))
    {
        handle_append_command(cv,bc);
        tm.reset();
        return;
    }

    // The second token is awlays the start of the object set.
    tm.next();
    int tok = tm.current_token();
    cv.set(Command_Flags::OBJECT, tok);
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
            if (cv.getIdentifier() == EMPTY) {
                Node_Value tmp;
                tmp.set_string(string_token);
                cv.set(Command_Flags::IDENTIFIER, tmp);
            }
            else {
                /* The only other TOKEN_DELIM that should be encountered before
                 * a preposition should be an argument (ie: database hash field)
                 */
                tmp.set_string(string_token);
                cv.set(Command_Flags::ARG, tmp);
            }
        }
        else if (is_state_token(tok)) {
            tmp.set_int(tok);
            cv.set(Command_Flags::ARG, tmp);
        }
        else if (isPrepositionToken(tok)) {
            bool do_not_reduce = false;
            /* First, check to see if the next value is 'location' */
            tm.next();
            if (tm.current_token() == Mogu_Syntax::location)
            {
                do_not_reduce = true;
            } else {
                tm.prev();
            }
            tm.save_location(); // We've done what we need with the preceeding
            tm.truncateHead(); // tokens, so get rid of them.

            // Simply join the remaining tokens together again as a string.
            // And set this as the "value" in CommadValue
            if (do_not_reduce)
            {
                tm.begin();
                std::stringstream buf;
                do {
                    int tok = tm.current_token();
                    if (tok == Mogu_Syntax::TOKEN_DELIM)
                        buf << tm.fetch_string();
                    else
                        buf << Mogu_Syntax::get(tok).str;
                    buf << " ";
                    tm.next();
                } while (tm.current_token() != Mogu_Syntax::OUT_OF_RANGE_END);
                std::string s = buf.str();
                s = s.substr(0,s.size()-1);
                cv.set(Command_Flags::VALUE, s);
            }
            else
            {
                tm.end(); // Allow tm to treat it as standard input.
                reduce_expressions(bc);
                if (tm.current_token() == Mogu_Syntax::TOKEN_DELIM)
                {
                    tmp.set_string(tm.fetch_string());
                    cv.set(Command_Flags::VALUE, tmp);
                }
                else {
                    // Any integral value here will be a true integer, not MoguSyn.
                    tmp.set_int((int)tm.current_token());
                    cv.set(Command_Flags::VALUE, tmp);
                }
            }
            break; // After parsing a prepositional, there is nothing left to do.
        }
        tm.next();
    }
    tm.reset();
}

}	// namespace Parsers
