/*
 * MoguScript_Tokenizer.h
 *
 *  Created on: Sep 11, 2012
 *      Author: tom
 */

#ifndef MOGUSCRIPT_TOKENIZER_H_
#define MOGUSCRIPT_TOKENIZER_H_

#include <declarations.h>
#include <Parsers/TokenGenerator.h>

namespace Parsers{

class MoguScript_Tokenizer : public TokenGenerator
{
public:
	MoguScript_Tokenizer (std::string value = EMPTY) : TokenGenerator(value)
	{
		add_pair('{','}'); // For Enums
		add_pair('^','^'); // For ints
		add_pair('`','`'); // For files
		add_pair('@','@'); // For nodes
		add_pair('~','~'); // For floats
		add_pair('$','$'); // For states
	}

	/*!\brief If the first token is a string, the entire thing is treated
	 * as a string. Otherwise, it goes through the standard tokenizing process.
	 * This prevents confusion when using token wrappers within normal strings,
	 * as there is no circumstance within Mogu that allows for a mixture of
	 * both string values and interpreted values.
	 *
	 * @param delimiter (optional) delimiter char
	 * @return The next token, or the original string if the first token is
	 * a string.
	 */
	virtual std::string next (char delimiter = ' ');

};

}//namespace Parsers

#endif /* MOGUSCRIPT_TOKENIZER_H_ */
