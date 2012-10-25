/*
 * FinalParsers.cpp
 *
 *  Created on: Oct 25, 2012
 *      Author: tom
 */

#ifndef FINALPARSERS_CPP_
#define FINALPARSERS_CPP_

#include <Parsers/NodeValueParser/NVPAutomaton.h>
#include <hash.h>
#include <Sessions/Submission.h>
#include <Exceptions/Exceptions.h>
#include <Sessions/Lookups.h>
#include <Redis/RedisCore.h>
#include <Mogu.h>
#include <Parsers/Parsers.h>

namespace Parsers
{


inline std::string convert_nv_to_string(Nodes::NodeValue& v)
{
	switch(v.getType())
	{
	case Nodes::int_value:
		return itoa(v.getInt());
	default:
		return v.getString();
	}
}

void parse_single_static_token (
		Nodes::NodeValue& v
		,std::string& input
		,NodeValueParser::Outputs tokentype
		,Goo::Moldable* broadcaster,
		int(*enumcallback)(const std::string&))
{
	std::string inputcpy = input;
	if (tokentype != NodeValueParser::string_value)
	{
		inputcpy = input.substr(1,input.length()-2);
	}
	else
	{
		v.setString(inputcpy);
		return;
	}

	switch (tokentype)
	{
	case NodeValueParser::integer_value:
		v.setInt(atoi(inputcpy.c_str()));
		return;
	case NodeValueParser::float_value:
		v.setFloat(atof(inputcpy.c_str()));
		return;
	case NodeValueParser::hashed_string:
		v.setString(Hash::toHash(inputcpy));
		return;
	case NodeValueParser::enum_value:
		if (enumcallback == 0)
		{
			throw Exceptions::Err_NoCallbackGiven ("parse_single_static_token");
		}
		v.setInt(enumcallback(inputcpy));
		return;
	default:
		v.setString(inputcpy);
		return;
	}
}

void parse_session_node (
		Nodes::NodeValue& v
		,std::string& base
		,std::string arg
		,NodeValueParser::Outputs arg_type
		)
{
	if (arg != EMPTY)
	{
		Nodes::NodeValue varg;
		parse_single_static_token(varg,arg,arg_type);
		arg = convert_nv_to_string(varg);
	}
	std::string uw_base = base.substr(1,base.length()-2);
	std::string value = Sessions::SubmissionHandler::dynamicLookup(uw_base, arg);
	NodeValueParser p(value,v);
}

void parse_data_node(
		Nodes::NodeValue& v
		,std::string& base
		,std::string arg
		,NodeValueParser::Outputs arg_type)
{
	using namespace Enums::NodeValueTypes::RedisTypes;
	mApp;
	std::string uw_base = base.substr(1, base.length()-2);
	app->redisCommand("type", uw_base);
	std::string node_type = Redis::toString(app->reply());

	if (arg != EMPTY)
	{
		Nodes::NodeValue varg;
		parse_single_static_token(varg,arg,arg_type);
		arg = convert_nv_to_string(varg);
	}

	if (node_type == REDIS_STR)
	{
		app->redisCommand("get", uw_base);
	}
	else if (node_type == REDIS_HSH)
	{
		app->redisCommand("hget", uw_base, arg);
	}
	else //(node_type == REDIS_LST)
	{
		app->redisCommand("lindex", uw_base, arg);
	}

	std::string response = Redis::toString(app->reply());
	NodeValueParser(response,v);
}

void parse_widget_state (
		Nodes::NodeValue& v
		,std::string& widget_identifier
		,std::string& state
		,Goo::Moldable* broadcaster
		)
{
	std::string uw_state = state.substr(1,state.length()-2);
	WidgetStateParser p;

	if (widget_identifier == "{self}")
	{
		broadcaster->getState(p.parse(uw_state),v);
	}
	else
	{
		mApp;
		std::string uw_id = widget_identifier.substr(1,widget_identifier.length()-2);
		Goo::Moldable* widget = app->registeredWidget(uw_id);
		widget->getState(p.parse(uw_state),v);
	}
}

}//namespace Parsers


#endif /* FINALPARSERS_CPP_ */