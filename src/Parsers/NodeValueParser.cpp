/*
 * NodeValueParser.cpp
 *
 *  Created on: Sep 10, 2012
 *      Author: tom
 */

#include <Parsers/NodeValueParser.h>
#include <Parsers/MoguScript_Tokenizer.h>
#include <Redis/RedisCore.h>
#include <Parsers/TokenTestPackage.h>
#include <Types/NodeValue.h>
#include <sstream>

namespace Parsers{

using Nodes::NodeValue;
using namespace Enums::NodeValueTypes;
using Goo::Moldable;

inline std::string itoa(int i)
{
	std::stringstream s;
	s << i;
	return s.str();
}

TokenTestResult __test_t1(TokenTestPackage* pkg)
{
	using namespace RedisTypes;
	if (pkg->__val_type != node_value)
	{
		pkg->interpret(VAL);
		return CPL;
	}
	else
	{
		Redis::command("type", pkg->__val);
		pkg->__r_node_type = Redis::toString();
		if (pkg->__r_node_type == REDIS_STR)
		{
			Redis::command("get", pkg->__val);
			std::string r_node_rslt = Redis::toString();
			NodeValueParser recursive_parser(
					r_node_rslt
					,pkg->__nval_final
					,pkg->__broadcaster
					,pkg->__callback);
			return recursive_parser.getResult();
		}
		else
		{
			return NXT_NODE;
		}
	}
}

TokenTestResult __test_t2(TokenTestPackage* pkg)
{
	using namespace Enums::NodeValueTypes::RedisTypes;

	/* We can't get information from a list with a string.
	 * Something is wrong. */

	std::string result;
	if ( pkg->__r_node_type == REDIS_LST
		&& (pkg->__val_type == string_value)) return ERR;

	pkg->interpret(ARG);
	if (pkg->__r_node_type == REDIS_LST)
	{
		std::string _arg = itoa(pkg->__nval_final->getInt());
		Redis::command("lindex", pkg->__val, _arg);
		result = Redis::toString();
	}

	if (pkg->__r_node_type == REDIS_HSH)
	{
		std::string _arg;
		if (pkg->__val_type == string_value)
		{
			_arg = pkg->__args[0];
		}
		else
		{
			_arg = itoa(pkg->__nval_final->getInt());
		}
		Redis::command("hget", pkg->__val, _arg);
		result = Redis::toString();
	}

	NodeValueParser recursive_parser(
			result
			, pkg->__nval_final
			, pkg->__broadcaster
			, pkg->__callback);
	return recursive_parser.getResult();
}

inline NodeValueTypes getMoguType(std::string token)
{
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
	return ntype;
}

NodeValueParser::NodeValueParser(
		std::string full_value,
		NodeValue* nval,
		Moldable* broadcaster,
		int(*callback)(const std::string&))
{

	__m_token_test t_tests[2] = {
			&__test_t1,
			&__test_t2
	};
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
		if (token != EMPTY)
		{
			std::string uw_token = tokenizer.isWrapped(token)?
					unwrap(token) : token;

			declaration.push_back(
					std::make_pair(
							uw_token,getMoguType(token)
			));
		}

	} while (token != EMPTY);

	TokenTestPackage pkg(nval);
	pkg.__broadcaster 		= broadcaster;
	pkg.__callback 			= callback;
	pkg.__val 				= declaration[0].first;
	pkg.__val_type 			= declaration[0].second;

	pkg.__last_result = t_tests[0](&pkg);

	size_t nargs =1;
	while (pkg.__last_result == NXT_NODE)
	{
		pkg.__args.push_back(declaration[nargs].first);
		pkg.__val_type = declaration[nargs].second;
		pkg.__last_result = t_tests[nargs](&pkg);
	}
}


}//namespace Parsers


