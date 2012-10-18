/*
 * Action_Emit.cpp
 *
 *  Created on: Oct 18, 2012
 *      Author: tom
 */

#include <Events/MoldableActionCenter.h>
#include <Wt/WEvent>
#include <Core/Moldable.h>

namespace Events
{
namespace ActionCenter
{
namespace Actions{

bool emit(Listeners& listeners, std::string& sig)
{
	size_t sz = listeners.size();
	for (size_t i = 0; i < sz; i++)
	{
		Goo::Moldable* widget = listeners.at(i);
		if (!widget->allowsAction(Enums::SignalActions::emit)) continue;
		if (widget == NULL) return false;

		if (sig == "click")
		{
			Wt::WMouseEvent m;
			widget->clicked().emit(m);
		}
	}
	return true;
}


}//namespace Actions
}//namespace ActionCenter
}//namespace Events


