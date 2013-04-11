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
#include <forward_list>

namespace Parsers {
	
class AttributeParser
{
	public:
		AttributeParser();
		void processInput(std::forward_list<int>& tokens, NodeValue& outputVal);
	
	private:

};

}	// namespace Parsers
#endif /* ATTRIBUTEPARSER_H_ */
