/*
 * Action_TestMatch.cpp
 *
 *  Created on: Oct 8, 2012
 *      Author: tom
 */

#include <Events/MoldableActionCenter.h>
#include <Types/NodeValue.h>
#include <Mogu.h>

namespace Events
{
namespace ActionCenter
{
namespace Actions
{
using Goo::Moldable;

bool test(Moldable& widget, Nodes::NodeValue& val)
{
	mApp;
	app->interpreter().giveInput(val.getString(), val);

	if (widget.allowsAction(Enums::SignalActions::match))
	{
		return val.getString().find(widget.valueCallback()) != std::string::npos;
	}

	return false;
}

}//Actions
}//ActionCenter
}//Events



