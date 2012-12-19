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
#include <Moldable/Implementations.h>
#include <Wt/WStackedWidget>
#include <Moldable/Moldable.h>
#include <Types/Listener.h>

namespace Events {
namespace ActionCenter {
namespace Actions {

inline void emitHiddenChangedSignals(Moldable& hidden, Moldable& visible)
{
    if (&hidden == &visible) return;
    hidden.hiddenChanged().emit();
    visible.hiddenChanged().emit();
}


void set_index(
    std::vector <Listener*>& listeners, int& new_index)
{
    size_t num_listeners = listeners.size();
    for (size_t i = 0; i < num_listeners; i++) {
        Listener* listener = listeners[i];
        MoldableStack& widget = (MoldableStack&) listener->getWidget();
        if (widget.allowsAction(Tokens::set_index)) {
            Wt::WStackedWidget* stack = (Wt::WStackedWidget*) widget.widget(0);
            Moldable* current = (Moldable*) stack->currentWidget();
            stack->setCurrentIndex(new_index);
            Moldable* next = (Moldable*) stack->currentWidget();
            emitHiddenChangedSignals(*current, *next);
            widget.stackIndexChanged().emit();
        }

    }
}

void increment_index(
    std::vector <Listener*>& listeners)
{
    int num_listeners = listeners.size();
    for (int w = 0; w < num_listeners; w++) {
        Listener* listener = listeners[w];
        MoldableStack& widget = (MoldableStack&) listener->getWidget();
        if (widget.allowsAction(Tokens::increment)) {
            Wt::WStackedWidget* stack = (Wt::WStackedWidget*) widget.widget(0);
            int current_index = stack->currentIndex();
            Moldable* current_widget = (Moldable*) stack->currentWidget();
            if (++current_index < stack->count()) {
                stack->setCurrentIndex(current_index);
                widget.stackIndexChanged().emit();
            }
            Moldable* next_widget = (Moldable*) stack->currentWidget();
            emitHiddenChangedSignals(*current_widget, *next_widget);
        }
    }
}

void decrement_index(
    std::vector <Listener*>& listeners)
{
    int num_listeners = listeners.size();
    for (int w = 0; w < num_listeners; w++) {
        Listener* listener = listeners[w];
        MoldableStack& widget = (MoldableStack&) listener->getWidget();
        if (widget.allowsAction(Tokens::decrement)) {
            Wt::WStackedWidget* stack = (Wt::WStackedWidget*) widget.widget(0);
            int current_index = stack->currentIndex();
            Moldable* current_widget = (Moldable*) stack->currentWidget();
            if (--current_index >= 0) {
                stack->setCurrentIndex(current_index);
                widget.stackIndexChanged().emit();
            }
            Moldable* next_widget = (Moldable*) stack->currentWidget();
            emitHiddenChangedSignals(*current_widget, *next_widget);
        }
    }
}

}    //namespace Actions
}    //namespace ActionCenter
}    //namespace Events

#endif /* ACTION_INDEXMUTATIONS_CPP_ */
