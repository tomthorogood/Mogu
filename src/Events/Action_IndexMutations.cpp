/*
 * Action_IndexMutations.cpp
 *
 *  Created on: Sep 5, 2012
 *      Author: tom
 */

#ifndef ACTION_INDEXMUTATIONS_CPP_
#define ACTION_INDEXMUTATIONS_CPP_

#include <Events/MoldableActionCenter.h>
#include <Events/BroadcastMessage.h>
#include <Wt/WStackedWidget>
#include <Core/Moldable.h>

namespace Events{
namespace ActionCenter{
namespace Actions{
using Goo::Moldable;
namespace Action = Enums::SignalActions;

void set_index (Listeners* listeners, int& new_index)
{
	int num_listeners = listeners->size();
	for (int i = 0; i < num_listeners; i++)
	{
		Moldable* widget = listeners->at(i);
		if (widget->allowsAction(Action::set_index))
		{
			Wt::WStackedWidget* stack = (Wt::WStackedWidget*)
					widget->widget(0);
			stack->setCurrentIndex(new_index);
			widget->stackIndexChanged().emit();
		}

	}
}

void increment_index(Listeners* listeners)
{
	int num_listeners = listeners->size();
	for (int w = 0; w < num_listeners; w++)
	{
		Moldable* widget = listeners->at(w);
		if (widget->allowsAction(Action::increment_index))
		{
			Wt::WStackedWidget* stack = (Wt::WStackedWidget*)
				widget->widget(0);
			int current_index = stack->currentIndex();
			if (++current_index < stack->count())
			{
				stack->setCurrentIndex(current_index);
				widget->stackIndexChanged().emit();
			}
		}
	}
}

void decrement_index(Listeners* listeners)
{
	int num_listeners = listeners->size();
	for (int w = 0; w < num_listeners; w++)
	{
		Moldable* widget = listeners->at(w);
		if (widget->allowsAction(Action::decrement_index))
		{
			Wt::WStackedWidget* stack = (Wt::WStackedWidget*)
					widget->widget(0);
			int current_index = stack->currentIndex();
			if (--current_index >=0)
			{
				stack->setCurrentIndex(current_index);
				widget->stackIndexChanged().emit();
			}
		}
	}
}


} //namespace Actions
} //namespace ActionCenter
} //namespace Events


#endif /* ACTION_INDEXMUTATIONS_CPP_ */
