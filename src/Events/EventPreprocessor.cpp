/*
 * EventPreprocessor.cpp
 *
 *  Created on: Oct 10, 2012
 *      Author: tom
 */

#include <Events/EventPreprocessor.h>
#include <Redis/RedisCore.h>
#include <Types/NodeValue.h>
#include <Parsers/MoguScript_Tokenizer.h>
#include <Exceptions/Exceptions.h>
#include <Mogu.h>

namespace Events {

using Exceptions::Err_MissingProperty;

std::string EventPreprocessor::getHashEntry(
    const std::string& node, const char* field)
{
    std::string result = EMPTY;
    mApp;
    app->redisExec(Mogu::Keep, "hexists %s %s", node.c_str(), field);
    if (redisReply_TRUE)
    {
        app->redisExec(Mogu::Keep, "hget %s %s", node.c_str(), field);
        result = redisReply_STRING;
    }
    return result;
}

EventPreprocessor::EventPreprocessor(
    std::string& node, int event_num)
{
    mApp;
    NodeValue v;

    MoguNode n_node(node);
    std::string i_event = std::to_string(event_num);
    std::string s_node = n_node.addSuffix(i_event);

    std::string trigger_str = getHashEntry(s_node, "trigger");
    std::string msg_str = getHashEntry(s_node, "message");
    if (msg_str == EMPTY) msg_str = " ";
    std::string action_str = getHashEntry(s_node, "action");
    message.original = msg_str;

    std::string listener_str = getHashEntry(s_node, "listener");
    if (listener_str == EMPTY)
        listener_str = getHashEntry(s_node, "listeners");

    std::string degrad_str = getHashEntry(s_node, "degradation");

    if (degrad_str != EMPTY) {
        app->interpreter().giveInput(degrad_str, v);
        degradation = v.getInt();
        std::string nxt_str = getHashEntry(s_node, "nextAction");

        if (nxt_str != EMPTY) {
            app->interpreter().giveInput(nxt_str, v, NONE);
            next_action = (Tokens::MoguTokens) v.getInt();
        }
        else
            next_action = Tokens::__NONE__;
    }
    else {
        degradation = 0;
        next_action = Tokens::__NONE__;
    }

    /*The trigger string will always be parseable to an enumerated type,
     * so we can do that now.
     */
    if (trigger_str == EMPTY) trigger_str = "{click}";
    app->interpreter().giveInput(trigger_str, v);
    trigger = (Tokens::MoguTokens) v.getInt();

    if (action_str == EMPTY) throw Err_MissingProperty(node, "action");
    app->interpreter().giveInput(action_str, v);
    action = (Tokens::MoguTokens) v.getInt();

    /* The listener will either be an enumerated type OR a string. Passing
     * it through the NVP will yield the final result and tell us what type
     * it parsed to. We can use that information to set up the requisite
     * fields in the preprocessor to avoid having to make this determination
     * at event firing.
     */
    if (listener_str == EMPTY) listener_str = "{self}";
    app->interpreter().giveInput(listener_str, v, NONE);

    if (v.getType() == string_value) {
        listener.s_listener = v.getString();
        listener.type = listener.string;
    }
    else {
        listener.f_listener = (Tokens::MoguTokens) v.getInt();
        listener.type = listener.widget;
    }

    Parsers::MoguScript_Tokenizer t(msg_str);
    if ((t.next() == msg_str) && t.startsWithString()) {
        message.status = message.processed;
    }
    else {
        message.status = message.delayed;
    }
    message.value.setString(msg_str);
}

Parsers::NodeValueParser::Outputs EventPreprocessor::getListenerType(
    std::string& str)
{
    Parsers::MoguScript_Tokenizer t(str);
    return Parsers::getMoguType(t.next());
}
}    //namespace Events
