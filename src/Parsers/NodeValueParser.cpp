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
#include <Sessions/Submission.h>
#include <Parsers/Parsers.h>
#include <Types/NodeValue.h>
#include <Core/Moldable.h>
#include <sstream>

#include <Wt/WApplication>
#include <Mogu.h>

namespace Parsers{

using Nodes::NodeValue;
using namespace Enums::NodeValueTypes;
using Goo::Moldable;

/* The first test determines whether we've reached a final type, or whether
 * we need to do more node lookups in order to find the values we seek.
 */
TokenTestResult __test_t1(TokenTestPackage& pkg)
{
	mApp;

	using namespace RedisTypes;
	if (
			pkg.__val_type != node_value
			&&pkg.__val_type != dynamic_storage)
	{
		pkg.interpret(VAL);
		if (pkg.__val_type <= float_value) return CPL;
		else if (pkg.__val_type == registry_value) return NXT_REG;
		else return NXT_CMD;
	}
	else // if (pkg.__val_type == node_value || pkg.__val_type = dynamic_storage)
	{
		if (pkg.__val_type == dynamic_storage)
		{
			pkg.__val = Sessions::SubmissionHandler::dynamicLookup(pkg.__val, EMPTY, NO_TRANSLATION);
			NodeValueParser recursive_parser(
					pkg.__val
					,pkg.__nval_final
					,pkg.__broadcaster
					,pkg.__callback);
			return recursive_parser.getResult();
		}
		/* Determine the type of the node we're dealing with */
		app->redisCommand("type", pkg.__val);
		pkg.__r_node_type = Redis::toString(app->reply());

		/* If the node is a string, node, it takes no arguments and
		 * can therefore be parsed without moving into __test_t2.
		 */
		if (pkg.__r_node_type == REDIS_STR)
		{
			app->redisCommand("get", pkg.__val);
			std::string r_node_rslt = Redis::toString(app->reply());
			NodeValueParser recursive_parser(
					r_node_rslt
					,pkg.__nval_final
					,pkg.__broadcaster
					,pkg.__callback);
			return recursive_parser.getResult();
		}
		else
		{
			return NXT_NODE;
		}
	}
}

/* This second test uses the second token taken from the syntax string and
 * based on its type determines the value thereof. This token will reside in
 * the 'arg' field of the test package, as it will be an argument of the
 * first token, which is either a node, registry value, or redis command.
 * Examples include:
 * 	@token1=node@ {enum_arg}
 * 	@token1=node@ $state_arg$
 * 	@token1=node@ hash_field
 * 	@token1=node@ ^index^
 * 	|named_widget| $state_arg$
 * 	!redis_command!| arg [...] #NOT CURRENTLY SUPPORTED#
 *
 */
TokenTestResult __test_t2(TokenTestPackage& pkg)
{
	using namespace Enums::NodeValueTypes::RedisTypes;


	mApp;
	std::string result;

	/* We can't get information from a list with a string.
	 * Something is wrong. */
	if ( pkg.__r_node_type == REDIS_LST
		&& (pkg.__val_type == string_value)) return ERR;

	pkg.interpret(ARG); // Parse the next argument and set it in __nval_final

	if (pkg.__r_node_type == REDIS_LST)
	{
		std::string _arg = itoa(pkg.__nval_final->getInt());
		app->redisCommand("lindex", pkg.__val, _arg);
		result = Redis::toString(app->reply());
	}

	if (pkg.__r_node_type == REDIS_HSH)
	{
		std::string _arg;
		if (pkg.__val_type == string_value)
		{
			_arg = pkg.__args[0];
		}
		else
		{
			_arg = itoa(pkg.__nval_final->getInt());
		}
		app->redisCommand("hget", pkg.__val, _arg);
		result = Redis::toString(app->reply());
	}

	NodeValueParser recursive_parser(
			result
			, pkg.__nval_final
			, pkg.__broadcaster
			, pkg.__callback);
	return recursive_parser.getResult();
}

TokenTestResult __test_tReg(TokenTestPackage& pkg)
{
	using namespace Enums::WidgetTypes;

	Enums::WidgetTypes::States s;
	WidgetStateParser p;
	s = p.parse(pkg.__args[0]);
	mApp;
	Moldable* w = app->registeredWidget(pkg.__val);
	w->getState(s, *(pkg.__nval_final));
	return CPL;
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
	case '|' : ntype = (fchar == '|')? registry_value : string_value;
		break;
	case '!' : ntype = (fchar == '!')? redis_command : string_value;
		break;
	case'['  : ntype = (fchar == ']')? dynamic_storage : string_value;
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
	:	declaration()
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

	/* Populate the 'TokenTestPackage' with current values, which will
	 * be manipulated by the tests in order to logically parse the values
	 * into a final resolution. The TokenTestPackage is there to avoid
	 * an ungodly amount of passed parameters.
	 */
	TokenTestPackage pkg(nval);
	pkg.__broadcaster 		= broadcaster;
	pkg.__callback 			= callback;
	pkg.__val 				= declaration[0].first;
	pkg.__val_type 			= declaration[0].second;

	/* The first test is required, and will set the stage for future
	 * tests.
	 */
	pkg.__last_result = t_tests[0](pkg);
	size_t nargs =1;

	/* Thereafter, we continue to parse the information in the package using
	 * test(x), where x is dynamically bound based on how many tests we've
	 * performed. (Super proud of this little block of code, by the way!)
	 */
	if (pkg.__last_result == NXT_NODE)
	{
		pkg.__args.push_back(declaration[nargs].first);
		pkg.__val_type = declaration[nargs].second;
		pkg.__last_result = t_tests[nargs](pkg);
	}
	else if (pkg.__last_result == NXT_REG)
	{
		pkg.__args.push_back(declaration[nargs].first);
		pkg.__val_type = declaration[nargs].second;
		pkg.__last_result = __test_tReg(pkg);
	}
}


}//namespace Parsers


