/*
 * EventPreprocessor.h
 *
 *  Created on: Oct 10, 2012
 *      Author: tom
 */

#ifndef EVENTPREPROCESSOR_H_
#define EVENTPREPROCESSOR_H_

#include <declarations.h>
#include <Types/NodeValue.h>
#include <Parsers/NodeValueParser.h>

namespace Events {
struct PreprocessedEventListener
{
    enum Ltype
    {
        string, widget,
    };

    Tokens::MoguTokens f_listener;
    std::string s_listener;
    Ltype type;

    PreprocessedEventListener()
    {
        s_listener = EMPTY;
        type = widget;
        f_listener = Tokens::self;
    }

    PreprocessedEventListener(
        std::string& listener)
    {
        type = string;
        s_listener = listener;
        f_listener = Tokens::self;
    }

    PreprocessedEventListener(
        Tokens::MoguTokens& listener)
    {
        type = widget;
        f_listener = listener;
    }
};

struct PreprocessedMessage
{
    enum Mtype
    {
        processed, delayed
    };

    Mtype status;
    std::string original;
    NodeValue value;
    PreprocessedMessage()
        : value()
    {
        status = delayed;
    }
};

struct EventPreprocessor
{
    inline std::string
        getHashEntry(const std::string& node, const std::string& field) {
        return getHashEntry(node, field.c_str());
    }
    std::string getHashEntry(const std::string& node, const char* field);
    Tokens::MoguTokens trigger, action, next_action;

    PreprocessedEventListener listener;
    PreprocessedMessage message;
    Parsers::NodeValueParser::Outputs getListenerType(
        std::string& str);
    int degradation;

    EventPreprocessor(
        std::string& node, int event_num);

};

}    //namespace Events
#endif /* EVENTPREPROCESSOR_H_ */
