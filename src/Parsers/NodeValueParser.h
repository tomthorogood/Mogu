/*
 * NodeValueParser.h
 *
 *  Created on: Sep 10, 2012
 *      Author: tom
 */

#ifndef NODEVALUEPARSER_H_
#define NODEVALUEPARSER_H_

#include <declarations.h>

namespace Parsers{

class NodeValueParser
{
private:
	std::vector <WrappedValue*> wrappedValues;
	Nodes::NodeValue* parsedValue;
	char __first_char;

public:
	NodeValueParser(
			std::string full_value,
			Nodes::NodeValue* node_value,
			Goo::Moldable* broadcaster =0);
	Nodes::NodeValue* getValue();
};

}//namespace Parsers


#endif /* NODEVALUEPARSER_H_ */
