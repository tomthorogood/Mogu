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

void set_index (Listeners* listeners, BroadcastMessage* broadcast)
{
	int num_listeners = listeners->size();
	for (int i = 0; i < num_listeners; i++)
	{
		Moldable* widget = listeners->at(i);
		if (widget->allowsAction(Action::set_index))
		{
			int new_index = broadcast->getMessage()->getInt();
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
#ifdef DEBUG
			std::cout << "Setting index of " << widget->getNodeList()->at(0) << std::endl;
#endif
			Wt::WStackedWidget* stack = (Wt::WStackedWidget*)
				widget->widget(0);
			int current_index = stack->currentIndex();
#ifdef DEBUG
			std::cout << "Current Index: " << current_index << std::endl;
			std::cout << "Next Index: " << current_index-1 << std::endl;
			std::cout << "Max Index: " << stack->count() << std::endl;
#endif
			if (++current_index < stack->count())
			{
#ifdef DEBUG
				std::cout << "Changing index to  ";
				std::cout << current_index << std::endl;
#endif
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
	#ifdef DEBUG
				std::cout << "CHANGING INDEX OF";
				std::cout << widget->getNodeList()->at(0) << " TO ";
				std::cout << current_index << std::endl;
	#endif
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
