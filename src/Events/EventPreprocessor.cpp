/*
 * EventPreprocessor.cpp
 *
 *  Created on: Oct 10, 2012
 *      Author: tom
 */

#include <Events/EventPreprocessor.h>
#include <Parsers/StyleParser.h>
#include <Parsers/Parsers.h>
#include <Redis/RedisCore.h>
#include <Types/NodeValue.h>
#include <Parsers/MoguScript_Tokenizer.h>
#include <Exceptions/Exceptions.h>

namespace Events{

using namespace Enums::SignalActions;
using namespace Enums::SignalTriggers;
using namespace Parsers::StyleParser;
using Exceptions::Err_MissingProperty;

inline std::string valOrEmpty(
		const std::string& node
		, const char* field
		, std::string alternate=EMPTY)
{
	return (widgetHasProperty(node, field)) ?
				getWidgetProperty(node, field) : alternate;

}

EventPreprocessor::EventPreprocessor (std::string& node)
{
	mApp;
	NodeValue v;

	std::string trigger_str		= getHashEntry(node, "trigger");
	std::string msg_str			= getHashEntry(node, "message", " ");
	std::string action_str		= getHashEntry(node, "action");
	message.original = msg_str;

	std::string listener_str = getHashEntry(node, "listener");
	if (listener_str == EMPTY) listener_str = getHashEntry(node, "listeners");

	std::string degrad_str = getHashEntry(node, "degradation");

	if (degrad_str != EMPTY)
	{
		app->interpreter().giveInput(degrad_str, v);
		degradation = v.getInt();
		std::string nxt_str = getHashEntry(node, "nextAction");

		if (nxt_str != EMPTY)
		{
			app->interpreter().giveInput(
					nxt_str
					,v
					,NONE
					,&Parsers::enum_callback
						<Parsers::SignalActionParser>);
			next_action = (SignalAction) v.getInt();
		}
		else next_action = (Enums::SignalActions::SignalAction) 0;
	}
	else
	{
		degradation = 0;
		next_action = Enums::SignalActions::NO_ACTION;
	}

	/*The trigger string will always be parseable to an enumerated type,
	 * so we can do that now.
	 */
	if (trigger_str == EMPTY) trigger_str = "{click}";
	app->interpreter().giveInput(
			trigger_str,
			v,
			NONE
			,&Parsers::enum_callback <Parsers::SignalTriggerParser>);
	trigger = (SignalTrigger) v.getInt();

	if (action_str == EMPTY) throw Err_MissingProperty(node, "action");
	app->interpreter().giveInput(
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
	if (listener_str == EMPTY) listener_str = "{self}";
	app->interpreter().giveInput(
			listener_str
			,v
			,NONE
			,Parsers::enum_callback <Parsers::FamilyMemberParser>);

	if (v.getType() == string_value)
	{
		listener.s_listener = v.getString();
		listener.type = listener.string;
	}
	else
	{
		listener.f_listener = (Enums::Family::_Family) v.getInt();
		listener.type = listener.widget;
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
