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

namespace Events{
struct PreprocessedEventListener
{
	enum Ltype { registry, family };
	Enums::Family::_Family f_listener;
	std::string r_listener;
	Ltype type;

	PreprocessedEventListener() {
		r_listener = EMPTY;
		type = family;
		f_listener = Enums::Family::self;
	}

	PreprocessedEventListener(const std::string& listener)
	{
		type = registry;
		r_listener = listener;
		f_listener = Enums::Family::self;
	}

	PreprocessedEventListener(Enums::Family::_Family& listener)
	{
		type = family;
		f_listener = listener;
	}
};

struct PreprocessedMessage
{
	enum Mtype { processed, delayed };
	Mtype status;
	std::string original;
	Nodes::NodeValue value;
	PreprocessedMessage() : value() {
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
	int degradation;

	EventPreprocessor (const std::string& node);
};

}//namespace Events
#endif /* EVENTPREPROCESSOR_H_ */
