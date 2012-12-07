/*SN_2_0_FF_PC
 * BroadcastMessage.cpp
 *
 *  Created on: Jul 12, 2012
 *      Author: tom
 */

#include <Events/BroadcastMessage.h>
#include <Core/Moldable.h>
#include <Events/EventPreprocessor.h>
#include <Parsers/Parsers.h>
#include <Parsers/MoguScript_Tokenizer.h>
#include <stdio.h>
#include <Mogu.h>
#include <Types/Listener.h>

namespace Events {
using std::string;
using Goo::Moldable;

namespace Action = Enums::SignalActions;
namespace Labels = Enums::Labels;

BroadcastMessage::BroadcastMessage(
    Goo::Moldable* _broadcaster, EventPreprocessor* preproc)
    : listeners()
{
    mApp;
    properties = preproc;
    broadcaster = _broadcaster;

    /* If the message type was anything other than a standard string,
     * we will now process that message.
     */
    if (preproc->message.status == preproc->message.delayed) {
#ifdef DEBUG
//		assert(_broadcaster != NULL);
        std::cout << "Parsing delayed message: " << preproc->message.original
            << std::endl;
#endif
        app->interpreter().giveInput(preproc->message.original,
            preproc->message.value, broadcaster);
    }
}

BroadcastMessage::~BroadcastMessage()
{
    size_t count = listeners.size();
    for (size_t i = 0; i < count; ++i) {
        delete listeners[i];
    }
}

void BroadcastMessage::updateAction()
{
    if (properties->next_action == Enums::SignalActions::NO_ACTION) return;

    Enums::SignalActions::SignalAction current = properties->action;
    properties->action = properties->next_action;
    properties->next_action = current;
}

void BroadcastMessage::resolveListeners()
{
    using namespace Enums::SignalActions;
    listeners.clear();
    /*!\brief First, determine if we need to deal with any bubbling or
     * trickling effects.
     */
    int degradation = properties->degradation;
    Goo::Moldable* iter_widget = broadcaster;

    while (--degradation >= 0) {
        if (properties->action == bubble) {
            iter_widget = (Goo::Moldable*) iter_widget->parent();
        }
    }

    if (properties->listener.s_listener != EMPTY) {
        std::string str = properties->listener.s_listener;
        Parsers::MoguScript_Tokenizer t(properties->listener.s_listener);
        if (t.next() != EMPTY && t.startsWithString()) {
            mApp;
            iter_widget = app->registeredWidget(str);
            if (iter_widget == NULL) return;
        }
        else {
            listeners.push_back(new Listener(str));
            return;
        }
    }

    updateAction();
    switch (properties->listener.f_listener) {

    case Enums::Family::parent: {
        Goo::Moldable* m = (Goo::Moldable*) iter_widget->parent();
        listeners.push_back(new Listener(m));
        break;
    }

    case Enums::Family::siblings: {
        Goo::Moldable* parent = (Goo::Moldable*) iter_widget->parent();
        size_t num_children = parent->countMoldableChildren();
        for (size_t c = 0; c < num_children; ++c) {
            Goo::Moldable* w = parent->child(c);
            if (w != iter_widget) listeners.push_back(new Listener(w));
        }
        break;
    }

    case Enums::Family::children: {
        size_t num_children = iter_widget->countMoldableChildren();
        for (size_t c = 0; c < num_children; ++c)
            listeners.push_back(new Listener(iter_widget->child(c)));
        break;
    }

    default:
        listeners.push_back(new Listener(iter_widget));
    }
}

}    //namespace Events

