/*
 * NodeValueParser.cpp
 *
 *  Created on: March 31, 2013
 *      Author: cameron
 */
#include <Parsers/NodeValueParser.h>

#include <Parsers/NodeValueParser/StateParser.h>
#include <Parsers/NodeValueParser/MathParser.h>
#include <Parsers/NodeValueParser/AttributeParser.h>
#include <Parsers/NodeValueParser/CommandParser.h>

namespace Parsers {
/* things we need to do:
 * 1.  break the string into tokens
 * 2.  detect whether we have states we need to resolve in StateParser
 * 3.  detect whether we have math we need to resolve in MathParser
 * 4.  detect whether we need to use AttributeParser or CommandParser
 */

NodeValueParser::NodeValueParser()
{
	__tokenizer.add_pair('"', '"');		//for string literals
	__tokenizer.add_pair('(', ')');		//for math strings
}

void NodeValueParser::tokenizeInput(const char* input)
{
	int len = strlen(inputString);

	int strIndex = 0;

	while (strIndex < len) {
		char current_char = inputString[strIndex];

		// If we have encountered a digit, begin the loop to 
		// determine the start/end indexes of said digit.

		if isdigit(current_char) {

			int token_start = strIndex;    
			int token_iter = token_start+1;
			char next = inputString[token_iter];

		for (; isdigit(next); ++token_iter) {
			next = inputString[token_iter];
		}

		// Once we reach this point, the token_iterator is one past the end 
		// index of the valid digit.
		
		// In order to cast this as an int, we must create a substring.
		// Start by getting the length of the digit:
		int num_len = token_iter - token_start;
		char* digit = (char*) malloc(num_len);

		// Assign the chars to the proper indexes in the substring.
		for (int dig_index = 0; dig_index < num_len; ++dig_index) {
			int char_index = token_start + dig_index;
			digit[dig_index] = inputString[char_index];
		}
		
		// Convert to integer and append to the NV vector.
		int result = atoi(digit);
		tokenValues.push_back( NodeValue(result) ); 
		free(digit);

		// Advance the global iterator to the local iterator, 
		// which, remember is already once char past the digit.
		strIndex = token_iter;
	}
   
	else if (current_char == '"') {
		// Now we must do something similar to separate string literals.
		// *NOTE: This does not cover quoted strings within string literals. 
		// I don't feel like thinking *THAT* hard right now. I've only had
        // one cup of coffee.
        int token_start = strIndex;
        int token_iter = token_start +1;
        char next_char = inputString[token_iter];
        for (; next_char != '"'; ++token_iter) {
            next_char = inputString[token_iter];
        }
        int lit_len = token_iter - token_start;
        char* literal = (char*) malloc(lit_len);
        for (int lit_index = 0; lit_index < lit_len; ++lit_index) {
            int char_index = token_start + lit_index;
            literal[lit_index] = inputString[char_index];
        }
        tokenValues.push_back( NodeValue(literal) ); //type conversion to std::string
        free(literal);
        strIndex = token_iter;
    }
    
    else if (current_char != ' ') { // For all other characters.
        // Do the same exact thing, but searching for the next space.
        int token_start = strIndex;
        int token_iter = token_start +1;
        char next_char = inputString[token_iter];
        for (; next_char != ' '; ++token_iter) {
            next_char = inputString[token_iter];
        }
        int tok_len = token_iter - token_start;
        char* token = (char*) malloc(tok_len);
        for (int tok_index = 0; tok_index < tok_len; ++tok_index) {
            int char_index = token_start + tok_index;
            token[tok_index] = inputString[char_index];
        }
        tokenValues.push_back( NodeValue(token) ); //type conversino to std::string
        free(token);
        strIndex = token_iter;
    }
    else ++strIndex;
}

}

void NodeValueParser::giveInput(std::string input, NodeValue& v)
{
	__tokenizer.reset().newString(input);

	// convert the input tokens into NodeValues and push
	while(__tokenizer.hasNext()) {
		std::string stringToken = __tokenizer.next();
		NodeValue nvToken; 
		tempToken.setString(stringToken)
		__tokens.push_back(nvToken);
	}

	// run appropriate parsers
	if(hasStates())
		__stateParser.giveInput(__tokens);

	evaluateMath();	//decides whether to invoke mathparser on a token-by-token basis


	if(detectEventCommand())
		__cmdParser.giveInput(__tokens, v);
	else
		__attParser.giveInput(__tokens, v);
}

bool NodeValueParser::hasStates()
{
	// TODO: figure out the best way to detect states!
}

void NodeValueParser::evaluateMath()
{
	for(int i=0; i<__tokens.size(); i++) {
		if(__tokenizer.isWrapped(__tokens[i], '('))
				__mathParser.giveInput(__tokens[i]);
	}
}

bool NodeValueParser::detectEventCommand()
{
}

void NodeValueParser::__reset__()
{
	__tokenizer.reset();
	__tokens.clear();

}

}	// namespace Parsers
