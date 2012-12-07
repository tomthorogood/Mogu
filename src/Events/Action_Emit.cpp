/*
 * Action_Emit.cpp
 *
 *  Created on: Oct 18, 2012
 *      Author: tom
 */

#include <Events/MoldableActionCenter.h>
#include <Wt/WEvent>
#include <Core/Moldable.h>
#include <Types/Listener.h>

namespace Events {
namespace ActionCenter {
namespace Actions {

bool emit(
    Listeners& listeners, std::string& sig)
{
    size_t sz = listeners.size();
    for (size_t i = 0; i < sz; i++) {
        Listener& listener = *(listeners.at(i));
        if (listener.getType() != listener.widget) return false;
        Goo::Moldable& widget = listeners.at(i)->getWidget();
        if (!widget.allowsAction(Enums::SignalActions::emit)) continue;

        if (sig == "click") {
            Wt::WMouseEvent m;
            widget.clicked().emit(m);
        }
    }
    return true;
}

}    //namespace Actions
}    //namespace ActionCenter
}    //namespace Events

