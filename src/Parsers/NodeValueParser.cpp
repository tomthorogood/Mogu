/*
 * NodeValueParser.cpp
 *
 *  Created on: March 31, 2013
 *      Author: cameron
 */
#include <Parsers/NodeValueParser.h>
#include <Types/NodeValue.h>
#include <Types/syntax.h>
#include <Types/CommandValue.h>
#include <cctype>

//debug
#include <iostream>

namespace Parsers {

NodeValueParser::NodeValueParser() : __stateParser(__tm), __mathParser(__tm)
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

int NodeValueParser::find_full_quote(std::string quoted_string) {
    // Trim the leading quote.
    if (quoted_string.at(0) == '"')
        quoted_string = quoted_string.substr(1);

    int q = quoted_string.find_first_of('"');
    if ((size_t) q == std::string::npos) return 0;
    /* Determine whether the character preceeding the quotation mark is
     * an escape delimiting slash.
     */
    while (quoted_string.at(q-1) == '\\')
    {
        q = quoted_string.find('"',q+1);
    };
    return q;
}
void NodeValueParser::tokenizeInput(std::string input, bool setAtBeginning)
{
	size_t inputIndex = 0;
	size_t input_size = input.size(); // save from doing repeated calls.
    int endTokenIndex = 0;
	while(inputIndex < input_size)
	{
		if(input[inputIndex] == '"')
		    // Search from the next token to the next occurence of a quotation.
			endTokenIndex =
			    find_full_quote(input.substr(inputIndex+1))
			    + inputIndex
			    + 1;
		else {
		    // Search for the next space delimiter.
		    size_t search = input.find(' ',inputIndex);
		    if (search == std::string::npos)
		        endTokenIndex = input.size();
		    else
		        endTokenIndex = search - 1;
		}

		// Extract the entire token between the indexes
		std::string token = input.substr(inputIndex, endTokenIndex-inputIndex+1);

		if (token != "") // handle cases where two spaces were input.
		{
            if(isdigit(token[0]))
                __tm.addToken(std::stoi(token));
            else
                __tm.addToken(token);
		}
		// Consume any ' ' delimiters.
		inputIndex = endTokenIndex +2;
	}

	//tell TokenManager that we're done adding tokens
	if (setAtBeginning)
	    __tm.begin();
	else
	    __tm.end();
}

/* Iterates through a vector of tokens and applies the core NvP logic
 * to reduce them down to one expression.
 */
bool NodeValueParser::reduceExpressions(Moldable* bc)
{
    // If there is only one token, it cannot be reduced any further.
    // Leave it alone.
	if (__tm.size() == 1)
	{
	    __tm.begin();
	    return false;
	}
	int currToken = __tm.currentToken();
	bool hasPreposition = false;

	/* Iterate backwards through the vector of tokens until the
	 * 'Begin' token is reached.
	 */
	while(currToken != MoguSyntax::OUT_OF_RANGE_BEGIN)
	{
	    /* The '(' syntax will always signify a mathematical expression.
	     * Hand this job over to the MathParser.
	     */
		if(currToken == MoguSyntax::OPER_OPPAREN) 
			__mathParser.processInput();
			
		/*  Object tokens will refer to a database field or widget state
		 *  that will be reduced to a string or integer.
		 */
		else if(isObjectToken(currToken))
		{
				__stateParser.processInput(bc);
		}

		else if(isPrepositionToken(currToken))
			hasPreposition = true;

		__tm.prev();

		currToken = __tm.currentToken();
	}
    __tm.begin();

	return hasPreposition;
}

void NodeValueParser::giveInput(const std::string& input, NodeValue& nv, Moldable* bc)
{
    __input = input;
    if (input.empty())
    {
        nv.setString("");
        return;
    }
	tokenizeInput(input);
	reduceExpressions(bc);

	while (__tm.currentToken()==MoguSyntax::TOKEN_DELIM
	    && __tm.fetchStringToken().at(0)!='"')
	{
	    std::string new_input = __tm.fetchStringToken();
	    __tm.reset();
	    tokenizeInput(new_input);
	    reduceExpressions(bc);
	}

	//if we have more than one token in __numTokens at this point (or
	//two if the first token is TOKEN DELIM), something has gone wrong

	if(__tm.currentToken() == MoguSyntax::TOKEN_DELIM)
		nv.setString(__tm.fetchStringToken());
	else
		nv.setInt((int)__tm.currentToken());
	__tm.reset(); // Clear the TokenManager to prepare for the next input

}

void NodeValueParser::setCommandValueObject(CommandValue& cv, bool r_tokens)
{
    int currTok;
    NodeValue tmp;

    CommandFlags obj_flag    = r_tokens ? CommandFlags::R_OBJECT :
        CommandFlags::OBJECT;
    CommandFlags id_flag     = r_tokens ? CommandFlags::R_IDENTIFIER :
        CommandFlags::IDENTIFIER;
    CommandFlags arg_flag    = r_tokens ? CommandFlags::R_ARG :
        CommandFlags::ARG;

    // Fast forward to the next object token.
    do {
        currTok = __tm.currentToken();
        __tm.next();
    } while (!isObjectToken(currTok));
    __tm.prev();

    cv.set(obj_flag, currTok);
    __tm.next();

    // If the next token is a string, it is necessarily an identifier.
    // It can also be another arbitrary token, however, representing a 
    // state/attribute("own content", or "app path"). In the latter case,
    // we need nothing more, though, so we return.
    currTok = __tm.currentToken();
    if (currTok == MoguSyntax::TOKEN_DELIM)
    {
        tmp.setString(__tm.fetchStringToken());
        cv.set(id_flag, tmp);
    }
    else if (currTok != MoguSyntax::OUT_OF_RANGE_END)
    {
        tmp.setInt( (int) currTok );
        cv.set(arg_flag, tmp);
        return;
    }

    // If we've reached this part, we're setting an arg, no ifs, ands, or buts.
    __tm.next();
    currTok = __tm.currentToken();
    if (currTok == MoguSyntax::TOKEN_DELIM)
        tmp.setString(__tm.fetchStringToken());
    else
        tmp.setInt( (int) currTok);
    cv.set(arg_flag, tmp);
}

void NodeValueParser::handleAppendCommand(CommandValue& cv, Moldable* bc)
{
    int token;
    NodeValue tmpValue;
    std::string str;
    uint8_t flags = cv.getFlags();
    bool preposition_found = false;

    // This is used for checking unexpected input to see if it's actually
    // supposed to be represented as an integer used as a list index.
    bool check_if_list;

    if (__tm.currentToken() != MoguSyntax::append) return;
    __tm.next();
    token = __tm.currentToken();

    // Cycle through the input, testing flag combinations and setting 
    // things where appropriate, until we're out of tokens.
    while (token != MoguSyntax::OUT_OF_RANGE_END)
    {
        // A string will either be a value or an identifier.
        if (MoguSyntax::TOKEN_DELIM == token)
        {
            str = __tm.fetchStringToken();
            tmpValue.setString(str);
            // If the only thing that has been set is the action, 
            // then the token delimiter will be a quoted string.
            if ((flags == (uint8_t)CommandFlags::ACTION) && isQuotedString(str))
            {
                flags = cv.set(CommandFlags::VALUE, tmpValue);
            }
            // If the R_OBJECT has been set, but there's no identifier,
            // the next string encountered must be the identifier.
            else if (cv.test(CommandFlags::R_OBJECT) && 
                    !cv.test(CommandFlags::R_IDENTIFIER))
            {
                flags = cv.set(CommandFlags::R_IDENTIFIER, tmpValue);
            }
            // Same as above, but for standard objects.
            else if (cv.test(CommandFlags::OBJECT) && 
                    !cv.test(CommandFlags::IDENTIFIER))
            {
                flags = cv.set(CommandFlags::IDENTIFIER, tmpValue);
            }
            // In all other cases, this is unexpected input, but it might
            // be a list index. 'continue' not present here because we want to
            // keep moving through the decision tree.
            else check_if_list = true;
        }
        // Object tokens will either be the OBJECT or R_OBJECT, depending
        // on if it was reached before the preposition or not.
        else if (isObjectToken(token))
        {
            if (!preposition_found && !cv.test(CommandFlags::R_OBJECT))
            {
                flags = cv.set(CommandFlags::R_OBJECT, token);
            }
            else if (preposition_found && !cv.test(CommandFlags::OBJECT))
            {
                flags = cv.set(CommandFlags::OBJECT, token);
            }
            else
                check_if_list = true;
                // Do not continue.
        }
        else if (isStateToken(token))
        {
            tmpValue.setInt((int) token);
            if (!preposition_found && !cv.test(CommandFlags::R_ARG))
            {
                flags = cv.set(CommandFlags::R_ARG, tmpValue);
            }
            else if (preposition_found && !cv.test(CommandFlags::ARG))
            {
                flags = cv.set(CommandFlags::ARG, token);
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
            __tm.prev();
            if (MoguSyntax::list != __tm.currentToken())
                return;
            __tm.next(); // Make sure to go back where we were in the input.
            tmpValue.setInt((int) token);
            if (!preposition_found && !cv.test(CommandFlags::R_ARG)) 
            {
                cv.set(CommandFlags::R_ARG, tmpValue);
            }
            else if (preposition_found && !cv.test(CommandFlags::ARG))
            {
                cv.set(CommandFlags::ARG, tmpValue);
            }
            // We have no clue what the hell this is. 
            else return;
        }

        __tm.next();
        token = __tm.currentToken();
    } // end while loop

    // After this, there is the possibility that we'll have a reduceable value
    // with R_OBJ/R_ID/R_ARG. If so, we'll go ahead and reduce that now:
    if (cv.objectIsReduceable(true))
    {
        cv.reduceObject(true, bc);
    }
       
}

void NodeValueParser::giveInput(const std::string& input, CommandValue& cv,
    Moldable* bc)
{
    NodeValue tmp;
    __input = input;
    tokenizeInput(input, true); // Make sure to return the iterator to
                                // the BEGINNING
#ifdef DEBUG
    __tm.printTokens();
#endif
    // The first token is always an action
    cv.set(CommandFlags::ACTION, __tm.currentToken());

    if (MoguSyntax::append == cv.get(CommandFlags::ACTION))
    {
        handleAppendCommand(cv,bc);
        __tm.reset();
        return;
    }

    // The second token is awlays the start of the object set.
    __tm.next();
    int tok = __tm.currentToken();
    cv.set(CommandFlags::OBJECT, tok);
    __tm.next();

    while (tok != MoguSyntax::OUT_OF_RANGE_END)
    {
        tok = __tm.currentToken();
        if (tok == MoguSyntax::TOKEN_DELIM)
        {
            std::string string_token = __tm.fetchStringToken();
            if (__tm.isQuotedString(string_token)) {
                __tm.reset();
                return;
            }
            /* The identifier should always be the first TOKEN_DELIM
             * encountered.
             */
            if (cv.getIdentifier() == EMPTY) {
                NodeValue tmp;
                tmp.setString(string_token);
                cv.set(CommandFlags::IDENTIFIER, tmp);
            }
            else {
                /* The only other TOKEN_DELIM that should be encountered before
                 * a preposition should be an argument (ie: database hash field)
                 */
                tmp.setString(string_token);
                cv.set(CommandFlags::ARG, tmp);
            }
        }
        else if (isStateToken(tok)) {
            tmp.setInt(tok);
            cv.set(CommandFlags::ARG, tmp);
        }
        else if (isPrepositionToken(tok)) {
            __tm.saveLocation(); // We've done what we need with the preceeding
            __tm.truncateHead(); // tokens, so get rid of them.
#ifdef DEBUG
            __tm.printTokens();
#endif
            __tm.end(); // Allow __tm to treat it as standard input.
            reduceExpressions(bc);
            if (__tm.currentToken() == MoguSyntax::TOKEN_DELIM)
            {
                tmp.setString(__tm.fetchStringToken());
                cv.set(CommandFlags::VALUE, tmp);
            }
            else {
                // Any integral value here will be a true integer, not MoguSyn.
                tmp.setInt((int)__tm.currentToken());
                cv.set(CommandFlags::VALUE, tmp);
            }
            break; // After parsing a prepositional, there is nothing left to do.
        }
        __tm.next();
    }
    __tm.reset();
}

}	// namespace Parsers
