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

    Enums::Family::_Family f_listener;
    std::string s_listener;
    Ltype type;

    PreprocessedEventListener()
    {
        s_listener = EMPTY;
        type = widget;
        f_listener = Enums::Family::self;
    }

    PreprocessedEventListener(
        std::string& listener)
    {
        type = string;
        s_listener = listener;
        f_listener = Enums::Family::self;
    }

    PreprocessedEventListener(
        Enums::Family::_Family& listener)
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
    Enums::SignalTriggers::SignalTrigger trigger;
    Enums::SignalActions::SignalAction action;
    Enums::SignalActions::SignalAction next_action;
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
