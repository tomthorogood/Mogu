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

	MoguDeclaration declaration;
	Nodes::NodeValue* parsedValue;


	inline std::string unwrap(std::string str)
	{
		return str.substr(1,str.length()-2);
	}

public:
	NodeValueParser(
			std::string full_value,
			Nodes::NodeValue* node_value,
			Goo::Moldable* broadcaster =0,
			int(*callback)(const std::string&) =0);
	Nodes::NodeValue* getValue();
};

}//namespace Parsers


#endif /* NODEVALUEPARSER_H_ */
