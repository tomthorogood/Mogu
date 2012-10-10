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
using namespace Parsers::StyleParser;

EventPreprocessor::EventPreprocessor (const std::string& node)
{
	std::string trigger_str 	= getWidgetProperty(node, "trigger");
	std::string action_str		= getWidgetProperty(node, "action");
	std::string msg_str			= getWidgetProperty(node, "message");
	std::string listener_str 	= getWidgetProperty(node, "listeners");

	Nodes::NodeValue v;

	if (widgetHasProperty(node, "degradation"))
	{
		std::string degrad_str = getWidgetProperty(node, "degradation");
		Parsers::NodeValueParser degrad_p(
				degrad_str
				,&v
				,NONE);
		degradation = (uint16_t) v.getInt();
		if (widgetHasProperty(node, "nextAction"))
		{
			std::string nxt_str = getWidgetProperty(node, "nextAction");
			Parsers::NodeValueParser nxt_p(
					nxt_str
					,&v
					,NONE
					,&Parsers::enum_callback
						<Parsers::SignalActionParser>);
			next_action = (SignalAction) v.getInt();
		} else next_action = 0;
	} else
	{
		degradation = 0;
		next_action = 0;
	}

	/*The trigger string will always be parseable to an enumerated type,
	 * so we can do that now.
	 */
	Parsers::NodeValueParser trigger_p(
			trigger_str,
			&v,
			NONE
			,&Parsers::enum_callback <Parsers::SignalTriggerParser>);
	trigger = (SignalTrigger) v.getInt();


	/*The action string will always be parseable to an enumerated type,
	 * so we can do that now.
	 */
	Parsers::NodeValueParser action_p(
			action_str
			,&v
			,NONE
			,&Parsers::enum_callback <Parsers::SignalActionParser>);
	action = (SignalAction) v.getInt();

	/* The listener will either be an enumerated type OR a string. Passing
	 * it through the NVP will yield the final result and tell us what type
	 * it parsed to. We can use that information to set up the requisite
	 * fields in the preprocessor to avoid having to make this determination
	 * at event firing.
	 */
	Parsers::NodeValueParser listener_p(
			listener_str
			,&v
			,NONE
			,&Parsers::enum_callback <Parsers::FamilyMemberParser>);

	/* The message is highly variable. However, if it is just a string, we
	 * will not have to do any further processing on it when the event fires.
	 * Because the MoguScript_Tokenizer will return the full string as next() if
	 * the first value is not a parseable Mogu type, we can thus easily
	 * determine whether this message is a string, or whether it will
	 * require processing once the event fires.
	 */
	if (v.getType() == Nodes::string_value)
	{
		listener.r_listener = v.getString();
		listener.type = listener.registry;
	}
	else
	{
		listener.f_listener = v.getInt();
		listener.type = listener.family;
	}

	Parsers::MoguScript_Tokenizer t(msg_str);
	if (t.next() == msg_str)
	{
		message.status = message.processed;
	}
	else
	{
		message.status = message.delayed;
	}
	message.value.setString(msg_str);


}

}
