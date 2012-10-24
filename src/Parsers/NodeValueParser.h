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

inline Enums::NodeValueTypes::NodeValueTypes getMoguType(std::string token)
{
	using namespace Enums::NodeValueTypes;
	char char0 = token.at(0);
	char fchar = token.at(token.length()-1);
	NodeValueTypes ntype;
	switch(char0)
	{
	case '{' : ntype = (fchar == '}')? enum_value : string_value;
		break;
	case '^' : ntype = (fchar == '^')? integer_value : string_value;
		break;
	case '@' : ntype = (fchar == '@')? node_value : string_value;
		break;
	case '~' : ntype = (fchar == '~')? float_value : string_value;
		break;
	case '$' : ntype = (fchar == '$')? widget_state : string_value;
		break;
	case '|' : ntype = (fchar == '|')? registry_value : string_value;
		break;
	case '!' : ntype = (fchar == '!')? redis_command : string_value;
		break;
	case '[' : ntype = (fchar == ']')? dynamic_storage : string_value;
		break;
	case '&' : ntype = (fchar == '&')? hashed_string : string_value;
		break;
	default:
		ntype = string_value;
	}
	return ntype;
}


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
