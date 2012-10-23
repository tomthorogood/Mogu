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
	std::string orig_string;

	inline std::string unwrap(std::string str)
	{
		return str.substr(1,str.length()-2);
	}
	Enums::NodeValueTypes::TokenTestResult __last_rslt;
public:
	NodeValueParser(
			std::string full_value,
			Nodes::NodeValue& node_value,
			Goo::Moldable* broadcaster =0,
			int(*callback)(const std::string&) =0);
	inline Enums::NodeValueTypes::TokenTestResult getResult()
		{ return __last_rslt;}
};

}//namespace Parsers


#endif /* NODEVALUEPARSER_H_ */
