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


void NodeValueParser::tokenizeInput(std::string input, bool setAtBeginning)
{
	size_t inputIndex = 0;
	size_t input_size = input.size(); // save from doing repeated calls.
    int endTokenIndex = 0;
	while(inputIndex < input_size)
	{
		if(input[inputIndex] == '"')
		    // Search from the next token to the next occurence of a quotation.
			endTokenIndex = input.find('"', inputIndex+1);
		else {
		    // Search for the next space delimiter.
		    size_t search = input.find(' ',inputIndex);
		    if (search == std::string::npos) break;
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
	//MoguSyntax lastToken = MoguSyntax::__NONE__;
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
    __tm.next();    /* Since the 'reduceExpressions function iterates to before
                     * the first token, we have to advance back to the first
                     * token before interacting with it.
                     */

	return hasPreposition;
}

void NodeValueParser::giveInput(std::string input, NodeValue& nv, Moldable* bc)
{
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
    tokenizeInput(input, true); // Make sure to return the iterator to
                                // the BEGINNING

    // The first token is always an action
    cv.setAction(__tm.currentToken<MoguSyntax>());

    // The second token is awlays the start of the object set.
    __tm.next();
    cv.setObject(__tm.currentToken<MoguSyntax>());

    /* Before we continue, we're going to clear the tokens we've already
     * used, so we can more easily determine what to do next.
     */
    __tm.saveLocation();
    __tm.next();
    __tm.deleteToSaved();

    while (__tm.currentToken <int>() != (int)TokenManager::OutOfRange::End)
    {
        MoguSyntax token = __tm.currentToken<MoguSyntax>();
        if (token == MoguSyntax::TOKEN_DELIM)
        {
            std::string string_token = __tm.fetchStringToken();
            // There should never be a quoted literal before the preposition
            if (__tm.isQuotedString()) {
                __tm.reset();
                return;
            }
            /* The identifier shoudl always be the first TOKEN_DELIM
             * encountered.
             */
            if (cv.getIdentifier() == EMPTY) {
                cv.setIdentifier(string_token);
            }
            else {
                /* The only other TOKEN_DELIM that should be encountered before
                 * a preposition should be an argument (ie: database hash field)
                 */
                cv.getArg().setString(string_token);
            }
        }
        else if (isStateToken(token)) {
            cv.getArg().setInt((int) token);
        }
        else if (isPrepositionToken(token)) {

            __tm.saveLocation();
            __tm.truncateHead();
            __tm.end(); // Allow __tm to treat it as standard input.
            reduceExpressions(bc);
            if (__tm.currentToken <MoguSyntax>() == MoguSyntax::TOKEN_DELIM)
                cv.getValue().setString(__tm.fetchStringToken());
            else
                // Any integral value here will be a true integer, not MoguSyn.
                cv.getValue().setInt(__tm.currentToken<int>());
            break; // After parsing a prepositional, there is nothing left to do.
        }
        __tm.next();
    }
    __tm.reset();
}

}	// namespace Parsers
