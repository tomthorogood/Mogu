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
#include <Parsers/Parsers.h>
#include <Parsers/StyleParser.h>
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
namespace Action = Enums::SignalActions;
namespace Family = Enums::Family;

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
    namespace Action = Enums::SignalActions;
    size_t num_listeners = broadcast.listeners.size();
    NodeValue& message = broadcast.properties->message.value;
    Action::SignalAction action = broadcast.properties->action;

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
            if (&(broadcast.listeners[i]->getWidget()) != NULL) {
                std::cout << broadcast.listeners[i]->getWidget().getNode();
            }
            else {
                std::cout << broadcast.listeners[i]->getString();
            }
            std::cout << " ";
        }
        std::cout << std::endl;
    }
#endif

    switch (action) {
    case Action::set_internal_path: {
        std::string path = message.getString();
        app->setInternalPath(path);
        break;
    }

    case Action::register_user: {
        handleBoolAction(broadcast, &Actions::register_user);
        break;
    }

    case Action::change_session: {
        handleBoolAction(broadcast, &Actions::change_session);
        break;
    }

    case Action::activate_global: {
        mApp;
        app->setSessionID("global");
        break;
    }

    case Action::javascript: {
        std::string script = message.getString();
        app->doJavaScript(script);
        break;
    }

    case Action::email_user: {
        std::string emailmsg = message.getString();
        Actions::EmailPacket pkt;
        std::string URL = app->bookmarkUrl();
        pkt.subject = "New eMail from " + URL;
        pkt.message = emailmsg;
        Actions::email_current_user(&pkt);
        break;
    }

    case Action::reset_password: {
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
    case Action::set_style: {

        string new_style = message.getString();
        Wt::WString wNewStyle(new_style);

        for (size_t w = 0; w < num_listeners; w++) {
            Moldable& widget = broadcast.listeners[w]->getWidget();
            if (widget.allowsAction(Action::set_style))
                widget.setStyleClass(wNewStyle);
        }
        break;
    }

    case Action::emit: {
        Actions::emit(broadcast.listeners, message.getString());
        break;
    }

    case Action::change_group: {
        app->setGroup(message.getString());
        break;
    }

    case Action::add_self_to_group: {
        const std::string& moguid = app->getUserManager().getMoguID();
        std::string& groupid = broadcast.listeners[0]->getString();
        std::string& rank = message.getString();
        GroupManager g(groupid);
        std::string origin = app->getUserManager().getUserSessions()[0];
        g.addMember(moguid, rank, origin);
        break;
    }

    case Action::add_other_to_group: {
        std::string moguid = app->getSlots()["USERID"];
        std::string& groupid = broadcast.listeners[0]->getString();
        std::string& rank = message.getString();
        GroupManager g(groupid);
        std::string origin = app->getUserManager().getUserSessions()[0];
        g.addMember(moguid, rank, origin);
        break;
    }

        /* Change the widget index of the listeners' stacked widgets. */
    case Action::set_index:
        Actions::set_index(broadcast.listeners, message.getInt());
        break;

        /* Append a new CSS class to the current widget style. */
    case Action::add_class: {
        for (size_t w = 0; w < num_listeners; w++) {
            Moldable& widget = broadcast.listeners[w]->getWidget();
            if (widget.allowsAction(Action::add_class)) {
                Wt::WString addl_style = message.getString();
                widget.addStyleClass(addl_style);
            }
        }
        break;
    }

        /* Remove a CSS class from the current widget style. */
    case Action::remove_class: {
        for (size_t w = 0; w < num_listeners; w++) {
            Moldable& widget = broadcast.listeners[w]->getWidget();
            if (widget.allowsAction(Action::remove_class)) {
                Wt::WString msg = message.getString();
                widget.removeStyleClass(msg);
            }
        }
        break;
    }

        /* Advance the stack's widget index. */
    case Action::increment_index:
        Actions::increment_index(broadcast.listeners);
        break;

        /* Rewind the stack's widget index. */
    case Action::decrement_index:
        Actions::decrement_index(broadcast.listeners);
        break;

        /* Adds a new widget to the tree. */
    case Action::add_widget: {
        for (size_t w = 0; w < num_listeners; w++) {
            Moldable& widget = broadcast.listeners[w]->getWidget();
            NodeValue v;
            app->interpreter().giveInput(message.getString(), v);

            if (widget.allowsAction(Action::add_widget)) {
                MoguNode n_template(v.getString());
                widget.addWidget(
                    app->getFactory().createMoldableWidget(
                        n_template.addPrefix("widgets")));
            }
        }
        break;
    }

        /* Removes a widget from the tree. */
        /* Note: Does not delete the widget! (?) */
    /*case Action::remove_child: {
        for (size_t w = 0; w < num_listeners; w++) {
            Moldable& widget = broadcast.listeners[w]->getWidget();
            if (widget.allowsAction(Action::remove_child)) {
                Moldable* child = NULL;
                ReadType msg_type;
                msg_type = message.getType();
                if (msg_type == string_value) {
                    std::string msg = message.getString();
                    if (app->widgetIsRegistered(msg)) {
                        child = app->registeredWidget(msg);
                    }
                }
                else {
                    int msg = message.getInt();
                    child = (Moldable*) widget.widget(msg);
                }
               // if (child != NULL) {
               //     widget.removeChild(child);
               // }
            }

        }
        break;
    }*/

    case Action::clear: {
        for (size_t w = 0; w < num_listeners; w++) {
            Moldable& widget = broadcast.listeners[w]->getWidget();
            if (widget.allowsAction(Action::clear)) {
                widget.clear();
            }
        }
        break;
    }

        /* 'listeners' here are the policy names and additional field arugments.
         * The message is the data to be stored. If 'listeners' contains only a
         * single argument, the data cannot be given to a hash node.
         */
    case Action::store_value:
        Actions::store(broadcast.listeners, message);
        break;

    case Action::test: {
        NodeValue listener_;
        NodeValue message_;
        app->interpreter()
            .giveInput(broadcast.listeners[0]->getString()
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

    case Action::slot: {
        for (size_t w = 0; w < num_listeners; w++) {
            Moldable& widget = broadcast.listeners[w]->getWidget();
            if (widget.allowsAction(Action::slot)) {
                std::string slot_ = message.getString();
                std::string value_ = widget.moldableValue();
                app->getSlots()[slot_] = value_;
            }
        }
        break;
    }

    case Action::match:
    case Action::test_text:
        if (broadcast.broadcaster->moldableValue() == EMPTY) break;
        Actions::test(broadcast.listeners[0]->getWidget(), message) ?
            broadcast.broadcaster->succeed().emit() :
            broadcast.broadcaster->fail().emit();
        break;

    case Action::set_text: {
        for (size_t w = 0; w < num_listeners; w++) {
            Moldable& widget = broadcast.listeners[w]->getWidget();
            if (widget.allowsAction(Action::set_text)) {
                widget.setMoldableValue(message.getString());
            }
        }
        break;
    }

    case Action::reload: {
        for (size_t w = 0; w < num_listeners; w++) {
            Moldable& widget = broadcast.listeners[w]->getWidget();
            widget.clear();
            widget.load();
        }
        break;
    }

    case Action::STDOUT: {
        for (size_t w = 0; w < num_listeners; w++) {
            Moldable& widget = broadcast.listeners[w]->getWidget();
            std::cout << widget.getNode() << " says: ";
            std::cout << message.getString() << std::endl;
        }
        break;
    }

    default:
        return;    // Don't do anything unexpected!
    }
}

}    //Namespace ActionCenter
}    //Namespace Events
