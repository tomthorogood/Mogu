/*
 * NodeValueParser.cpp
 *
 *  Created on: March 31, 2013
 *      Author: cameron
 */
#include <Parsers/NodeValueParser.h>
#include <Types/NodeValue.h>
#include <cctype>

//debug
#include <iostream>
#define PACKING_DELIMITER 999999

namespace Parsers {

NodeValueParser::NodeValueParser()
{
}

void NodeValueParser::tokenizeInput(const char* input)
{
	const char* inputIndex = input;
	std::forward_list<int> iter = __tokens.before_begin();

	while(*inputIndex != '\0')
	{
		if(isdigit(*inputIndex))
		{
			//delimit the digit by finding the next whitespace
			const char* endTokenIndex = strchr(inputIndex, ' ');

			//...or the null char, if we're at the end of string
			if(!endTokenIndex)	
				endTokenIndex = strchr(inputIndex, '\0');

			//copy the digit to a temp buffer to convert to int
			int tokenLength = endTokenIndex - inputIndex + 1;
			char token[tokenLength];
			memcpy(token, inputIndex, tokenLength-1);

			//make sure to add in a terminating char!
			token[tokenLength-1] = '\0';

			iter = __tokens.insert_after(iter, atoi(token));

			//move our index to the appropriate spot
			if(!(*endTokenIndex) == '\0')
				inputIndex = endTokenIndex + 1;
			else
				inputIndex = endTokenIndex;
		}
		else
		{
			//we have some sort of string; put in our special symbol
			iter = __tokens.insert_after(iter, PACKING_DELIMITER);

			//delimit our string by locating the last char before
			//whitespace
			const char* endTokenIndex;
			if(*inputIndex == '"')
				endTokenIndex = strchr(inputIndex+1, '"');
			else
			{
				endTokenIndex = strchr(inputIndex, ' ') - 1;
				if(!endTokenIndex)
					endTokenIndex = strchr(inputIndex, '\0');
			}

			//push the number of characters in the string on to our
			//tokens vector
			iter = __tokens.insert_after(iter, endTokenIndex - inputIndex + 1);

			//pack characters into ints by bitshifting the characters
			//as we see them, pushing the resultant int on to our
			//vector whenever it's full
			unsigned int packedInt = 0;
			int packedIntIndex = 0;
			while(inputIndex <= endTokenIndex)
			{
				packedInt |= *inputIndex << (8 * packedIntIndex);
				inputIndex++;

				if(packedIntIndex == 3)
				{
					iter = __tokens.insert_after(iter, packedInt);
					packedIntIndex = 0;
					packedInt = 0;
					continue;
				}

				packedIntIndex++;
			}

			//if we didn't end with a fully packed integer, make sure
			//to send it anyway
			if(packedIntIndex != 0)
				iter = __tokens.insert_after(iter, packedInt);

			inputIndex += 1;
		}
	}

}

void NodeValueParser::giveInput(const char* input, NodeValue& v)
{
	std::cout << "input string: " << input << std::endl << std::endl;
	tokenizeInput(input);


	std::cout << "tokens:";
	for(auto it = __tokens.begin(); it != __tokens.end(); ++it)
		std::cout << ' ' << *it;
	std::cout << std::endl;
}


}	// namespace Parsers
