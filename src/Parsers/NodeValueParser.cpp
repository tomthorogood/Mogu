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
		Moldable* broadcaster,
		int(*callback)(const std::string&))
{
	using namespace Enums::NodeValueTypes;

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

	NodeValueTypes t1_type = declaration[0].second;
	std::string t1 = declaration[0].first;

	switch(t1_type)
	{
	case string_value:
		nval->setString(t1);
		return;

	case integer_value:
		const char* c_str = t1.c_str();
		nval->setInt( atoi(c_str) );
		return;

	case float_value:
		const char* c_str =t1.c_str();
		nval->setFloat( atof(c_str) );
		return;

	case enum_value:
		int val = callback(t1);
		nval->setInt(val);
		return;

	case widget_state:
		parsedValue = broadcaster->getState(/*State*/);
	}


}


}//namespace Parsers


