/*
 * Math.h
 *
 *  Created on: March 12th, 2013
 *      Author: cameron
 */


namespace Parsers {

/*
 * let's assume that the python interpreter will do the following:
 * (enumerated values are just examples)
 *  	input (.mogu file): ((49 + 5) / 2) - 2
 *  	output (to db)    : "17 17 49 31 5 17 26 2 17 84 2"
 * 
 * 	we have a problem here.  how do we differentiate between the
 * 	enumerated values, and the integers themselves?  we either have to
 * 	mark the numbers before they go to the database, or the
 * 	mathematical symbols.  OR if we remove the possibility of
 * 	parentheses, we can assume that the first token is a number, the
 * 	second is an operation, the third is a number, etc., but I don't
 * 	like this approach.
 *
 * 	let's assume that we're marking the mathematical symbols for now -
 * 	there are fewer possibilities for those, and we know that anytime
 * 	they're used the string in question should be passing through
 * 	MathParser. 
 *
 * 	so the input into MathParser would look something like this:
 * 		input (from db): ^17 ^17 49 ^31 5 ^17 ^26 2 ^17 ^84 2
 * 				 output: 25
 *
 * basically do order-of-operations, and resolve parentheses.
 * can we leverage the fact that c++ already knows how to do
 * all of this?
 *
 * 1. 
 *
 */
	
class MathParser
{
	public:
		MathParser();
	
	private:
		std::vector<std::string> __mathTokens;
		

}

}	// namespace Parsers
