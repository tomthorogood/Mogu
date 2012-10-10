/*
 * Action_TestMatch.cpp
 *
 *  Created on: Oct 8, 2012
 *      Author: tom
 */

#include <Events/MoldableActionCenter.h>
#include <Types/NodeValue.h>
#include <Parsers/NodeValueParser.h>
#include <Events/BroadcastMessage.h>

namespace Events
{
namespace ActionCenter
{
namespace Actions
{
using Goo::Moldable;

bool test(Moldable& widget, Nodes::NodeValue& val)
{
	if (widget.allowsAction(Enums::SignalActions::match))
	{
		return widget.valueCallback() == val.getString();
	}
	return false;
}

}//Actions
}//ActionCenter
}//Events



