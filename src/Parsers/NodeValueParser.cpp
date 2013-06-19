/*
 * NodeValueParser.cpp
 *
 *  Created on: March 31, 2013
 *      Author: cameron
 */
#include <Parsers/NodeValueParser.h>
#include <Types/NodeValue.h>
#include <Types/syntax.h>
#include <Events/CommandValue.h>
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
	if (__tm.size() == 1) return false;
	MoguSyntax currToken = __tm.currentToken<MoguSyntax>();
	bool hasPreposition = false;

	/* Iterate backwards through the vector of tokens until the
	 * 'Begin' token is reached.
	 */
	while((int) currToken != (int) TokenManager::OutOfRange::Begin)
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
			//if(lastToken < MoguSyntax::OPER_OPPAREN)
				__stateParser.processInput(bc);
		}

		else if(isPrepositionToken(currToken))
			hasPreposition = true;

		//lastToken = __tm.currentToken<MoguSyntax>();
		__tm.prev();

		currToken = __tm.currentToken<MoguSyntax>();
	}
    __tm.begin();

	return hasPreposition;
}

void NodeValueParser::giveInput(std::string input, NodeValue& nv, Moldable* bc)
{
#ifdef DEBUG
    static size_t num_iters = 0;
    const char* current_input = input.c_str();
    ++num_iters;
#endif
	tokenizeInput(input);
	reduceExpressions(bc);

	//if we have more than one token in __numTokens at this point (or
	//two if the first token is TOKEN DELIM), something has gone wrong

	if(__tm.currentToken<MoguSyntax>() == MoguSyntax::TOKEN_DELIM)
		nv.setString(__tm.fetchStringToken());
	else
		nv.setInt(__tm.currentToken<int>());
	__tm.reset(); // Clear the TokenManager to prepare for the next input

}

void NodeValueParser::giveInput(std::string input, CommandValue& cv,
    Moldable* bc)
{
    NodeValue tmp;
    tokenizeInput(input, true); // Make sure to return the iterator to
                                // the BEGINNING
#ifdef DEBUG
    __tm.printTokens();
#endif
    // The first token is always an action
    cv.setAction(__tm.currentToken<MoguSyntax>());

    // The second token is awlays the start of the object set.
    __tm.next();
    cv.setObject(__tm.currentToken<MoguSyntax>());

    while (__tm.currentToken <int>() != (int)TokenManager::OutOfRange::End)
    {
        MoguSyntax token = __tm.currentToken<MoguSyntax>();
        if (token == MoguSyntax::TOKEN_DELIM)
        {
            std::string string_token = __tm.fetchStringToken();
            // There should never be a quoted literal before the preposition
            // ^^ THAT IS A LIE. FIX! ^^
            // ie: append "hello I am Tom" to widget foo text
            if (__tm.isQuotedString(string_token)) {
                __tm.reset();
                return;
            }
            /* The identifier should always be the first TOKEN_DELIM
             * encountered.
             */
            if (cv.getIdentifier() == EMPTY) {
                cv.setIdentifier(string_token);
            }
            else {
                /* The only other TOKEN_DELIM that should be encountered before
                 * a preposition should be an argument (ie: database hash field)
                 */
                tmp.setString(string_token);
                cv.setArg(tmp);
            }
        }
        else if (isStateToken(token)) {
            tmp.setInt((int) token);
            cv.setArg(tmp);
        }
        else if (isPrepositionToken(token)) {

            __tm.saveLocation(); // We've done what we need with the preceeding
            __tm.truncateHead(); // tokens, so get rid of them.
#ifdef DEBUG
            __tm.printTokens();
#endif
            __tm.end(); // Allow __tm to treat it as standard input.
            reduceExpressions(bc);
            if (__tm.currentToken <MoguSyntax>() == MoguSyntax::TOKEN_DELIM)
            {
                tmp.setString(__tm.fetchStringToken());
                cv.setValue(tmp);
            }
            else {
                // Any integral value here will be a true integer, not MoguSyn.
                tmp.setInt(__tm.currentToken<int>());
                cv.setValue(tmp);
            }
            break; // After parsing a prepositional, there is nothing left to do.
        }
        __tm.next();
    }
    __tm.reset();
}

}	// namespace Parsers
