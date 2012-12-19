/*
 * Action_Emit.cpp
 *
 *  Created on: Oct 18, 2012
 *      Author: tom
 */

#include <Events/MoldableActionCenter.h>
#include <Wt/WEvent>
#include <Moldable/Moldable.h>
#include <Types/Listener.h>

namespace Events {
namespace ActionCenter {
namespace Actions {

bool emit(
    std::vector<Listener*>& listeners, std::string& sig)
{
    size_t sz = listeners.size();
    for (size_t i = 0; i < sz; i++) {
        Listener* listener = listeners[i];
        if (listener->getType() != listener->widget) return false;
        Moldable& widget = listener->getWidget();
        if (!widget.allowsAction(Tokens::emit)) continue;

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

