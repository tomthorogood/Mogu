/*
 * AttributeParser.h
 *
 *  Created on: March 4th, 2013
 *      Author: cameron
 */

#include <Parsers/TokenGenerator.h>
#include <Types/NodeValue.h>
#include <string>

namespace Parsers {
	
/*
 * i'm assuming that we don't need to store any broadcaster data,
 * because the parsed data isn't going to be stored as an event.
 *
 */
class AttributeParser
{
	public:
		AttributeParser();
		void giveInput(std::string input, NodeValue& v);
		void giveInput(std::string input);
	
	private:
		TokenGenerator __tokenizer;
		NodeValue* __iovalue;

}

}	// namespace Parsers
