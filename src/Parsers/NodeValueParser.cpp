/*
 * NodeValueParser.cpp
 *
 *  Created on: Sep 10, 2012
 *      Author: tom
 */

#include <Parsers/NodeValueParser.h>
#include <Parsers/MoguScript_Tokenizer.h>

namespace Parsers{

using Nodes::NodeValue;
using Goo::Moldable;

NodeValueParser::NodeValueParser(
		std::string full_value,
		NodeValue* nval,
		Moldable* broadcaster)
{
	parsedValue = nval;
	MoguScript_Tokenizer tokenizer(full_value);
	std::string token = EMPTY;

	/* First, we are going to iterate over the parseable string, retrieving
	 * each token. Each token will be tested for its type, and the pair matching
	 * the token itself along with its type will be pushed into the declaration
	 * map.
	 */
	do
	{
		using namespace Enums::NodeValueTypes;
		token = tokenizer.next();
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
		default:
			ntype = string_value;
		}

		declaration.push_back( std::make_pair(unwrap(token),ntype) );

	} while (token != EMPTY);
}


}//namespace Parsers


