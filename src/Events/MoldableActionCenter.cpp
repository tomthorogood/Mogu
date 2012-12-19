/*
 * MoldableActionCenter.cpp
 *
 *  Created on: Jul 12, 2012
 *      Author: tom
 */

#include <Types/Listener.h>
#include <Types/GroupManager.h>
#include <Events/MoldableActionCenter.h>
#include <Events/BroadcastMessage.h>
#include <Events/EventPreprocessor.h>
#include <Parsers/TokenGenerator.h>
#include <Wt/WStackedWidget>
#include <Wt/WText>
#include <Factories/MoldableFactory.h>
#include <Moldable/Moldable.h>
#include <Moldable/Implementations.h>
#include <Static.h>
#include <Mogu.h>
#include <Wt/WString>
#include <stdlib.h>
#include <signal.h>

#include <Redis/StorageRequest.h>

namespace Events {
namespace ActionCenter {

using std::string;

inline void handleBoolAction(
    BroadcastMessage& message, bool (*callback)())
{
    if (callback()) {
        message.broadcaster->succeed().emit();
    }
    else
        message.broadcaster->fail().emit();
}

void submitBroadcast(
    BroadcastMessage& broadcast)
{
    broadcast.resolveListeners();
    directListeners(broadcast);
    broadcast.updateAction();
}

void directListeners(
    BroadcastMessage& broadcast)
{
    if (broadcast.listeners.size() <= 0) return;
    mApp;
    size_t num_listeners = broadcast.listeners.size();
    NodeValue& message = broadcast.properties->message.value;
    Tokens::MoguTokens action = broadcast.properties->action;

#ifdef DEBUG
    if (broadcast.broadcaster != 0) {
        std::cout << "Parsing Action " << action << " for ";
        std::cout << num_listeners << " listeners from ";
        std::cout << broadcast.broadcaster->getNode() << std::endl;
        std::cout << "Message: ";
        if (message.getType() == string_value)
            std::cout << message.getString();
        else
            std::cout << message.getInt();

        std::cout << std::endl;
        std::cout << "Listeners: ";
        for (size_t i = 0; i < num_listeners; ++i) {
            Listener* listener = broadcast.listeners[i];
            if (&(listener->getWidget()) != NULL) {
                std::cout << listener->getWidget().getNode();
            }
            else {
                std::cout << listener->getString();
            }
            std::cout << " ";
        }
        std::cout << std::endl;
    }
#endif

    switch (action) {
    case Tokens::set_internal_path: {
        std::string path = message.getString();
        app->setInternalPath(path);
        break;
    }

    case Tokens::register_user: {
        handleBoolAction(broadcast, &Actions::register_user);
        break;
    }

    case Tokens::change_session: {
        handleBoolAction(broadcast, &Actions::change_session);
        break;
    }

    case Tokens::activate_global: {
        mApp;
        app->setSessionID("global");
        break;
    }

    case Tokens::javascript: {
        std::string script = message.getString();
        app->doJavaScript(script);
        break;
    }

    case Tokens::email_user: {
        std::string emailmsg = message.getString();
        Actions::EmailPacket pkt;
        std::string URL = app->bookmarkUrl();
        pkt.subject = "New eMail from " + URL;
        pkt.message = emailmsg;
        Actions::email_current_user(&pkt);
        break;
    }

    case Tokens::reset_password: {
        std::string uid = app->getSlots()["USERID"];
        if (!Actions::reset_password(uid)) {
            broadcast.broadcaster->fail().emit();
        }
        else {
            broadcast.broadcaster->succeed().emit();
        }
        break;
    }

        /* Change the CSS style class of all listeners to the style
         * given as the broadcast message.
         */
    case Tokens::set_style: {

        string new_style = message.getString();
        Wt::WString wNewStyle(new_style);

        for (size_t w = 0; w < num_listeners; w++) {
            Listener* listener = broadcast.listeners[w];
            Moldable& widget = listener->getWidget();
            if (widget.allowsAction(Tokens::set_style))
                widget.setStyleClass(wNewStyle);
        }
        break;
    }

    case Tokens::emit: {
        Actions::emit(broadcast.listeners, message.getString());
        break;
    }

    case Tokens::change_group: {
        app->setGroup(message.getString());
        break;
    }

    case Tokens::add_self_to_group: {
        const std::string& moguid = app->getUserManager().getMoguID();
        Listener* listener = broadcast.listeners[0];
        std::string& groupid = listener->getString();
        std::string& rank = message.getString();
        GroupManager g(groupid);
        std::string origin = app->getUserManager().getUserSessions()[0];
        g.addMember(moguid, rank, origin);
        break;
    }

    case Tokens::add_other_to_group: {
        std::string moguid = app->getSlots()["USERID"];
        Listener* listener = broadcast.listeners[0];
        std::string& groupid = listener->getString();
        std::string& rank = message.getString();
        GroupManager g(groupid);
        std::string origin = app->getUserManager().getUserSessions()[0];
        g.addMember(moguid, rank, origin);
        break;
    }

        /* Change the widget index of the listeners' stacked widgets. */
    case Tokens::set_index:
        Actions::set_index(broadcast.listeners, message.getInt());
        break;

        /* Append a new CSS class to the current widget style. */
    case Tokens::add_class: {
        for (size_t w = 0; w < num_listeners; w++) {
            Listener* listener = broadcast.listeners[w];
            Moldable& widget =listener->getWidget();
            if (widget.allowsAction(Tokens::add_class)) {
                Wt::WString addl_style = message.getString();
                widget.addStyleClass(addl_style);
            }
        }
        break;
    }

        /* Remove a CSS class from the current widget style. */
    case Tokens::remove_class: {
        for (size_t w = 0; w < num_listeners; w++) {
            Listener* listener = broadcast.listeners[w];
            Moldable& widget = listener->getWidget();
            if (widget.allowsAction(Tokens::remove_class)) {
                Wt::WString msg = message.getString();
                widget.removeStyleClass(msg);
            }
        }
        break;
    }

        /* Advance the stack's widget index. */
    case Tokens::increment:
        Actions::increment_index(broadcast.listeners);
        break;

        /* Rewind the stack's widget index. */
    case Tokens::decrement:
        Actions::decrement_index(broadcast.listeners);
        break;

        /* Adds a new widget to the tree. */
    case Tokens::add_widget: {
        for (size_t w = 0; w < num_listeners; w++) {
            Listener* listener = broadcast.listeners[w];
            Moldable& widget = listener->getWidget();
            NodeValue v;
            app->interpreter().giveInput(message.getString(), v);

            if (widget.allowsAction(Tokens::add_widget)) {
                MoguNode n_template(v.getString());
                widget.addWidget(
                    app->getFactory().createMoldableWidget(
                        n_template.addPrefix("widgets")));
            }
        }
        break;
    }

    case Tokens::clear: {
        for (size_t w = 0; w < num_listeners; w++) {
            Listener* listener = broadcast.listeners[w];
            Moldable& widget = listener->getWidget();
            if (widget.allowsAction(Tokens::clear)) {
                widget.clear();
            }
        }
        break;
    }

        /* 'listeners' here are the policy names and additional field arugments.
         * The message is the data to be stored. If 'listeners' contains only a
         * single argument, the data cannot be given to a hash node.
         */
    case Tokens::store_value:
        Actions::store(broadcast.listeners, message);
        break;

    case Tokens::test: {
        NodeValue listener_;
        NodeValue message_;
        Listener* listener = broadcast.listeners[0];
        app->interpreter()
            .giveInput(listener->getString()
                , listener_, broadcast.broadcaster);
        if (message.getType() == string_value) {
            app->interpreter()
                .giveInput(message.getString(), message_, broadcast.broadcaster);
        }
        else message_.copy(&message);

        Actions::true_test(listener_, message_) ?
            broadcast.broadcaster->succeed().emit() :
            broadcast.broadcaster->fail().emit();
        break;}

    case Tokens::slot: {
        for (size_t w = 0; w < num_listeners; w++) {
            Listener* listener = broadcast.listeners[w];
            Moldable& widget = listener->getWidget();
            if (widget.allowsAction(Tokens::slot)) {
                std::string slot_ = message.getString();
                std::string value_ = widget.moldableValue();
                app->getSlots()[slot_] = value_;
            }
        }
        break;
    }

    case Tokens::set_text: {
        for (size_t w = 0; w < num_listeners; w++) {
            Listener* listener = broadcast.listeners[w];
            Moldable& widget = listener->getWidget();
            if (widget.allowsAction(Tokens::set_text)) {
                widget.setMoldableValue(message.getString());
            }
        }
        break;
    }

    case Tokens::reload: {
        for (size_t w = 0; w < num_listeners; w++) {
            Listener* listener = broadcast.listeners[w];
            Moldable& widget = listener->getWidget();
            widget.reload();
        }
        break;
    }

    default:
        return;    // Don't do anything unexpected!
    }
}

}    //Namespace ActionCenter
}    //Namespace Events
