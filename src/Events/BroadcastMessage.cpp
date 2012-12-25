/*SN_2_0_FF_PC
 * BroadcastMessage.cpp
 *
 *  Created on: Jul 12, 2012
 *      Author: tom
 */

#include <Events/BroadcastMessage.h>
#include <Moldable/Moldable.h>
#include <Moldable/Implementations.h>
#include <Events/EventPreprocessor.h>
#include <Parsers/MoguScript_Tokenizer.h>
#include <stdio.h>
#include <Mogu.h>
#include <Types/Listener.h>

namespace Events {
using std::string;

BroadcastMessage::BroadcastMessage(
    Moldable* _broadcaster, EventPreprocessor* preproc)
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
    if (properties->next_action == Tokens::__NONE__) return;

    Tokens::MoguTokens current = properties->action;
    properties->action = properties->next_action;
    properties->next_action = current;
}

void BroadcastMessage::resolveListeners()
{
    listeners.clear();
    /*!\brief First, determine if we need to deal with any bubbling or
     * trickling effects.
     */
    int degradation = properties->degradation;
    Moldable* iter_widget = broadcaster;

    while (--degradation >= 0) {
        if (properties->action == Tokens::bubble) {
            iter_widget = (Moldable*) iter_widget->parent();
        }
    }

    if (properties->listener.s_listener != EMPTY) {
        std::string str = properties->listener.s_listener;
        Parsers::MoguScript_Tokenizer t(properties->listener.s_listener);
        if (t.next() != EMPTY && t.startsWithString()) {
            mApp;
            iter_widget = app->registeredWidget(str);
            if (iter_widget == nullptr) return;
        }
        else {
            listeners.push_back(new Listener(str));
            return;
        }
    }

    updateAction();
    switch (properties->listener.f_listener) {

    case Tokens::parent: {
        Moldable* m = (Moldable*) iter_widget->parent();
        listeners.push_back(new Listener(m));
        break;
    }

    case Tokens::siblings: {
        MoldableAbstractParent* parent =
            static_cast <MoldableAbstractParent*>(iter_widget->parent());

        size_t num_children = parent->moldableChildren().size();;
        for (size_t c = 0; c < num_children; ++c) {
            Moldable* w = parent->moldableChild(c);
            if (w != iter_widget) listeners.push_back(new Listener(w));
        }
        break;
    }

    case Tokens::children: {
        MoldableAbstractParent* parent =
            static_cast <MoldableAbstractParent*>(iter_widget);
        size_t num_children = parent->moldableChildren().size();
        for (size_t c = 0; c < num_children; ++c)
            listeners.push_back(new Listener(parent->moldableChild(c)));
        break;
    }

    default:
        listeners.push_back(new Listener(iter_widget));
        break;
    }
}

}    //namespace Events

