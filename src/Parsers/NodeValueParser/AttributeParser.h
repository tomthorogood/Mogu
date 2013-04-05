/*
 * AttributeParser.h
 *
 *  Created on: March 4th, 2013
 *      Author: cameron
 */

#ifndef ATTRIBUTEPARSER_H_
#define ATTRIBUTEPARSER_H_

#include <declarations.h>
#include <Types/NodeValue.h>
#include <vector>

namespace Parsers {
	
/*
 * TODO:
 * 	- add logic to detect what type of node we are dealing with
 *	- accomodate possibility of db node being a list
 *	- use redisExec instead of redisCommand
 *	- use provided API to handle return of widget attributes
 *	- figure out how to access slot data
 *
 */
class AttributeParser
{
	public:
		AttributeParser();
		void giveInput(std::string input, NodeValue& v);
		inline void __reset__()
		{
			__tokens.clear();
			__iovalue = 0;
		}
	
	private:
		std::vector<std::string> __tokens;
		NodeValue* __iovalue;

};

}	// namespace Parsers
#endif /* ATTRIBUTEPARSER_H_ */
