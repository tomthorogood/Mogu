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
#include <Exceptions/Exceptions.h>
#include <Redis/RedisCore.h>
#include <Mogu.h>
#include <Redis/SessionValueRequest.h>
#include <Types/syntax.h>

namespace Parsers {

inline std::string convert_nv_to_string(
    NodeValue& v)
{
    switch (v.getType()) {
    case int_value:
        return itoa(v.getInt());
    default:
        return v.getString();
    }
}

void parse_single_static_token(
    NodeValue& v
    , std::string& input
    , NodeValueParser::Outputs tokentype)
{
    std::string inputcpy = input;
    if (tokentype != NodeValueParser::string_value) {
        inputcpy = input.substr(1, input.length() - 2);
    }
    else {
        v.setString(inputcpy);
        return;
    }

    switch (tokentype) {
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
        v.setInt((int) Tokens::syntaxMap.get(inputcpy));
        return;
    default:
        v.setString(inputcpy);
        return;
    }
}

void parse_session_node(
    NodeValue& v, std::string& base, std::string arg,
    NodeValueParser::Outputs arg_type)
{
    if (arg != EMPTY) {
        NodeValue varg;
        parse_single_static_token(varg, arg, arg_type);
        arg = convert_nv_to_string(varg);
    }
    std::string uw_base = base.substr(1, base.length() - 2);

    Redis::SessionValueRequest lookup(uw_base, arg);
    std::string value = lookup.getValue();

    //When recursing, we cannot use the same NVP instance as before!
    NodeValueParser p(value, v);
}

void parse_data_node(
    NodeValue& v, std::string& base, std::string arg,
    NodeValueParser::Outputs arg_type, Moldable* broadcaster)
{
    mApp;
    std::string uw_base = base.substr(1, base.length() - 2);
    const char* cbase = uw_base.c_str();
    app->redisCommand("type %s", cbase);
    std::string node_type = redisReply_STRING;

    if (arg != EMPTY) {
        NodeValue varg;
        if (arg_type == NodeValueParser::widget_state) {
            parse_widget_state(varg, "{self}", arg, broadcaster);
        }
        else {
            parse_single_static_token(varg, arg, arg_type);
        }
        arg = convert_nv_to_string(varg);
    }

    const char* carg = arg.c_str();

    if (node_type == Tokens::syntaxMap.get(Tokens::string)) {
        app->redisCommand("get %s", cbase);
    }
    else if (node_type == Tokens::syntaxMap.get(Tokens::hash)) {
        app->redisCommand("hget %s %s", cbase, carg);
    }
    else    //(node_type == REDIS_LST)
    {
        app->redisCommand("lindex %s %s", cbase, carg);
    }

    std::string response = Redis::toString(app->reply());
    //When recursing, we cannot use the same NVP instance as before!
    NodeValueParser p(response, v);
}

void parse_widget_state(
    NodeValue& v
    , std::string widget_identifier
    , std::string& state
    , Moldable* broadcaster)
{
    std::string uw_state = state.substr(1, state.length() - 2);

    Tokens::MoguTokens e_token = Tokens::syntaxMap.get(uw_state);

    if (widget_identifier == "{self}") {
        broadcaster->getState(e_token, v);
    }
    else {
        mApp;
        std::string uw_id = widget_identifier.substr(1,
            widget_identifier.length() - 2);
        Moldable* widget = app->registeredWidget(uw_id);
        widget->getState(e_token, v);
    }
}

}    //namespace Parsers

#endif /* FINALPARSERS_CPP_ */
