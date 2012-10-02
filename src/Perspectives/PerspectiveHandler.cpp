/*
 * PerspectiveHandler.cpp
 *
 *  Created on: Jul 30, 2012
 *      Author: tom
 */

#include <Perspectives/PerspectiveHandler.h>
#include <Redis/RedisCore.h>
#include <TurnLeftLib/Utils/HungryVector.h>
#include <Static.h>
#include <Mogu.h>
#include <Core/Moldable.h>
#include <Parsers/NodeValueParser.h>
#include <Parsers/Parsers.h>
#include <Types/NodeValue.h>
#include <Wt/WStackedWidget>
#include <Events/MoldableActionCenter.h>

namespace Perspective{
namespace Handler{


void mold(std::string perspective)
{
	namespace Action = Enums::SignalActions;
	std::string action_str;
	std::string message_str;
	int num_widgets = 0;
	Redis::strvector namespace_;

	mApp;
	app->redisCommand("keys","perspectives."+perspective+".*");
	Redis::toVector(app->reply(),namespace_);
	num_widgets = namespace_.size();

	for (int w = 0; w < num_widgets; w++)
	{
		std::string affect_node = namespace_.at(w);
		std::string name;
		std::string action_str;
		std::string message_str;
		Action::SignalAction action;
		Goo::Moldable* widget;

		app->redisCommand("hget",affect_node,"name");
		name = Redis::toString(app->reply());
		if (!app->widgetIsRegistered(name)) return;

		app->redisCommand("hget", affect_node, "action");
		action_str = Redis::toString(app->reply());

		app->redisCommand("hget", affect_node, "message");
		message_str = Redis::toString(app->reply());

		widget = app->registeredWidget(name);

		Nodes::NodeValue
			action_val,
			message_val;

		Parsers::NodeValueParser action_parser(action_str, &action_val,
				widget,
				&Parsers::enum_callback <Parsers::SignalActionParser>);

		Parsers::NodeValueParser message_parser(
				message_str, &message_val, widget);

		action = (Action::SignalAction)
				action_val.getInt();

		switch(action)
		{
		case Action::set_index:{
			if (widget->allowsAction(Action::set_index))
			{
				Wt::WStackedWidget* stack = (Wt::WStackedWidget*)
						widget->widget(0);
				stack->setCurrentIndex(message_val.getInt());
				break;
			}}
		case Action::set_style:{
			if (widget->allowsAction(Action::set_style))
			{
				widget->setStyleClass(message_val.getString());
				break;
			}}
		case Action::rebroadcast:{
			std::string _message = "events."+message_val.getString();
			Events::ActionCenter::submitBroadcast(
					Events::ActionCenter::generateNewBroadcast(
							widget, _message)
			);
			break;}
		default: break;
		}
	}
}

} //namespace Perspective
} //namespace Handler
