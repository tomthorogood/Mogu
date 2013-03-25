/*
 * AttributeParser.h
 *
 *  Created on: March 4th, 2013
 *      Author: cameron
 */

#include <declarations.h>
#include <Parsers/TokenGenerator.h>
#include <Types/NodeValue.h>
#include <vector>

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
		inline void reset()
		{
			__tokenizer.reset();
			__tokens.clear();
			__iovalue = 0;
		}
	
	private:
		TokenGenerator __tokenizer;
		std::vector<std::string> __tokens;
		NodeValue* __iovalue;

};

}	// namespace Parsers
