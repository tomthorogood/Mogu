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

namespace Perspective{
namespace Handler{


void mold(std::string perspective)
{
	Mogu* app = Application::mogu();
	namespace Action = Enums::SignalActions;
	std::string action_str;
	std::string message_str;
	int num_widgets = 0;
	Redis::strvector namespace_;


	Redis::command("keys","perspectives."+perspective+".*");
	Redis::toVector(namespace_);
	num_widgets = namespace_.size();

	for (int w = 0; w < num_widgets; w++)
	{
		std::string affect_node = namespace_.at(w);
		std::string name;
		std::string action_str;
		std::string message_str;
		Action::SignalAction action;
		Nodes::NodeValue* message;
		Goo::Moldable* widget;

		Redis::command("hget",affect_node,"name");
		name = Redis::toString();
		if (!app->widgetIsRegistered(name)) return;

		Redis::command("hget", affect_node, "action");
		action_str = Redis::toString();

		Redis::command("hget", affect_node, "message");
		message_str = Redis::toString();

		widget = app->registeredWidget(name);

		Parsers::NodeValueParser action_parser(action_str,
				widget,
				&Parsers::enum_callback <Parsers::SignalActionParser>);

		Parsers::NodeValueParser message_parser(message_str, widget);

		action = (Action::SignalAction)
				action_parser.getValue()->getInt();

		message = message_parser.getValue();

		switch(action)
		{
		case Action::set_index:{
			if (widget->allowsAction(Action::set_index))
			{
				Wt::WStackedWidget* stack = (Wt::WStackedWidget*)
						widget->widget(0);
				stack->setCurrentIndex(message->getInt());
				break;
			}}
		case Action::set_style:{
			if (widget->allowsAction(Action::set_style))
			{
				widget->setStyleClass(message->getString());
				break;
			}}
		default: break;
			}
	}


}

} //namespace Perspective
} //namespace Handler
