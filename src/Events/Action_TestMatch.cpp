/*
 * Action_TestMatch.cpp
 *
 *  Created on: Oct 8, 2012
 *      Author: tom
 */

#include <Events/MoldableActionCenter.h>

namespace Events
{
namespace ActionCenter
{
namespace Actions
{
using Goo::Moldable;

bool test(Moldable& widget, BroadcastMessage& broadcast)
{
	if (widget.allowsAction(Enums::SignalActions::match))
	{
		Nodes::NodeValue v;
		Parsers::NodeValueParser p(
				broadcast.getMessage()->getString(), &v);
		return widget.valueCallback() == v.getString();
	}
	return false;
}

}//Actions
}//ActionCenter
}//Events



