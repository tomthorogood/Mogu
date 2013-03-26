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
 * TODO:
 * 	- add logic to detect what type of node we are dealing with
 *	- accomodate possibility of db node being a list
 *	- use redisExec instead of redisCommand
 *	- prepare AttributeParser to access widget data (pending update)
 *	- figure out how to access slot data
 *	- actually use the github issue tracker instead of this
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
