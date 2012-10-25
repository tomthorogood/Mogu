/*
 * EventPreprocessor.cpp
 *
 *  Created on: Oct 10, 2012
 *      Author: tom
 */

#include <Events/EventPreprocessor.h>
#include <Parsers/NodeValueParser.h>
#include <Parsers/StyleParser.h>
#include <Parsers/Parsers.h>
#include <Redis/RedisCore.h>
#include <Types/NodeValue.h>
#include <Parsers/MoguScript_Tokenizer.h>

namespace Events{

using namespace Enums::SignalActions;
using namespace Enums::SignalTriggers;
using Parsers::NodeValueParser;
using namespace Parsers::StyleParser;


inline std::string valOrEmpty(const std::string& node, const char* field, std::string alternate=EMPTY)
{
	return (widgetHasField(node, field)) ?
				getWidgetField(node, field) : alternate;
}

EventPreprocessor::EventPreprocessor (const std::string& node)
{
	std::string trigger_str 	= valOrEmpty(node, "trigger");
	std::string msg_str			= valOrEmpty(node, "message", " ");
	std::string action_str		= getWidgetField(node, "action");
	message.original = msg_str;

	Nodes::NodeValue v;

	std::string listener_str = widgetHasField(node, "listener") ?
			getWidgetField(node, "listener")
			: getWidgetField(node, "listeners");

	if (widgetHasField(node, "degradation"))
	{
		std::string degrad_str = getWidgetField(node, "degradation");
		Parsers::NodeValueParser degrad_p(
				degrad_str
				,v
				,NONE);
		degradation = v.getInt();
		if (widgetHasField(node, "nextAction"))
		{
			std::string nxt_str = getWidgetField(node, "nextAction");
			Parsers::NodeValueParser nxt_p(
					nxt_str
					,v
					,NONE
					,&Parsers::enum_callback
						<Parsers::SignalActionParser>);
			next_action = (SignalAction) v.getInt();
		} else next_action = (Enums::SignalActions::SignalAction) 0;
	} else
	{
		degradation = 0;
		next_action = (Enums::SignalActions::SignalAction) 0;
	}

	/*The trigger string will always be parseable to an enumerated type,
	 * so we can do that now.
	 */
	if (trigger_str != EMPTY)
	{
		Parsers::NodeValueParser trigger_p(
				trigger_str,
				v,
				NONE
				,&Parsers::enum_callback <Parsers::SignalTriggerParser>);
		trigger = (SignalTrigger) v.getInt();
	}


	/*The action string will always be parseable to an enumerated type,
	 * so we can do that now.
	 */
	Parsers::NodeValueParser action_p(
			action_str
			,v
			,NONE
			,&Parsers::enum_callback <Parsers::SignalActionParser>);
	action = (SignalAction) v.getInt();

	/* The listener will either be an enumerated type OR a string. Passing
	 * it through the NVP will yield the final result and tell us what type
	 * it parsed to. We can use that information to set up the requisite
	 * fields in the preprocessor to avoid having to make this determination
	 * at event firing.
	 */

	if (getListenerType(listener_str) == NodeValueParser::dynamic_storage)
	{
		listener.s_listener = listener_str;
		listener.type = listener.string;
	}
	else
	{
		Parsers::NodeValueParser listener_p(
				listener_str
				,v
				,NONE
				,Parsers::enum_callback <Parsers::FamilyMemberParser>
		);

		/* The message is highly variable. However, if it is just a string, we
		 * will not have to do any further processing on it when the event fires.
		 * Because the MoguScript_Tokenizer will return the full string as next() if
		 * the first value is not a parseable Mogu type, we can thus easily
		 * determine whether this message is a string, or whether it will
		 * require processing once the event fires.
		 */
		if (v.getType() == Nodes::string_value)
		{
			listener.s_listener = v.getString();
			listener.type = listener.string;
		}
		else
		{
			listener.f_listener = (Enums::Family::_Family) v.getInt();
			listener.type = listener.widget;
		}

	}

	Parsers::MoguScript_Tokenizer t(msg_str);
	if ((t.next() == msg_str) && t.startsWithString())
	{
		message.status = message.processed;
	}
	else
	{
		message.status = message.delayed;
	}
	message.value.setString(msg_str);
}

Parsers::NodeValueParser::Outputs
EventPreprocessor::getListenerType(std::string& str)
{
	Parsers::MoguScript_Tokenizer t(str);
	return Parsers::getMoguType(t.next());
}
}//namespace Events
