/*
 * MoldableActionCenter.cpp
 *
 *  Created on: Jul 12, 2012
 *      Author: tom
 */

#include <Types/Listener.h>
#include <Events/MoldableActionCenter.h>
#include <Events/BroadcastMessage.h>
#include <Events/EventPreprocessor.h>
#include <Parsers/Parsers.h>
#include <Parsers/StyleParser.h>
#include <Parsers/TokenGenerator.h>
#include <Wt/WStackedWidget>
#include <Wt/WText>
#include <Core/Moldable.h>
#include <Core/MoldableTemplate.h>
#include <Core/MoldableFactory.h>
#include <Static.h>
#include <Mogu.h>
#include <Wt/WString>
#include <stdlib.h>
#include <signal.h>

#include <Redis/StorageRequest.h>

namespace Events
{
namespace ActionCenter
{

using std::string;
namespace Action = Enums::SignalActions;
namespace Family = Enums::Family;

using namespace Goo;

namespace {
    static ListenerMap listenerMap;
}

inline bool listenerIsString(BroadcastMessage& msg)
{
	return	msg.properties->listener.s_listener != EMPTY;
}

inline void handleBoolAction(
		BroadcastMessage& message, bool (*callback)())
{
	if (callback())
	{
		message.broadcaster->succeed().emit();
	}
	else message.broadcaster->fail().emit();
}

inline Listeners* getListenerVector(BroadcastMessage& broadcast)
{
	ListenerMap::iterator iter = listenerMap.find(&broadcast);
	if (iter == listenerMap.end())
	{
		listenerMap[&broadcast] = new Listeners();
	}
	return listenerMap[&broadcast];
}

inline void resolveListeners(Listeners* listeners, const std::string& name)
{
	mApp;
	if (app->widgetIsRegistered(name))
	{
		listeners->push_back(new Listener( app->registeredWidget(name) ));
	}
}

inline void resolveListeners(Listeners* listeners, BroadcastMessage& broadcast)
{
	int degradation = broadcast.properties->degradation;
	Family::_Family f = broadcast.properties->listener.f_listener;
	listeners->push_back(new Listener(broadcast.broadcaster));


    while (--degradation>=0)
    {
    	updateListeners(broadcast);
    }

    broadcast.updateAction();

    if (broadcast.properties->action == Action::rebroadcast)
    {
    	if (f != Family::application)
    	{
    		getNuclearFamily(broadcast);
    	}
    	string nodeName =
    			"events."+broadcast.properties->message.value.getString();
    	Listeners* listeners = listenerMap[&broadcast];
    	size_t num_listeners = listeners->size();
    	for (size_t listener = 0; listener < num_listeners; ++listener)
    	{
    		Moldable& __listener = listeners->at(listener)->getWidget();
    		EventPreprocessor preproc(nodeName);
    		BroadcastMessage newmsg(&__listener, &preproc);
    		submitBroadcast(newmsg);
    	}

    	cleanupBroadcast(broadcast);
        return; // Avoid side effects with recursion!
    }

    if (f != Family::application) getNuclearFamily(broadcast);
}

inline void resolveListeners(BroadcastMessage& broadcast)
{
	bool isString = listenerIsString(broadcast);
	Listeners* listeners = getListenerVector(broadcast);

	switch(broadcast.properties->action)
	{
	/* Determine what sort of listener we have. */
	case Enums::SignalActions::store_value:{
		listeners->push_back(new Listener(broadcast.properties->listener.s_listener));
		break;}
	default:
		if (isString) resolveListeners(
				listeners, broadcast.properties->listener.s_listener);
		else
			resolveListeners(listeners, broadcast);
	}
}

void submitBroadcast(BroadcastMessage& broadcast)
{
    resolveListeners(broadcast);
    directListeners(broadcast);
    cleanupBroadcast(broadcast);
}

void getNuclearFamily(BroadcastMessage& broadcast)
{
    namespace Family = Enums::Family;
    Family::_Family familyMembers = broadcast.properties->listener.f_listener;
    if (familyMembers == Family::self) return;

    Listeners* current_listeners = listenerMap[&broadcast];
    Listeners* new_listeners = new Listeners();

    unsigned int num_current = current_listeners->size();

    for (unsigned int c = 0; c < num_current; c++)
    {
        Moldable& listener = current_listeners->at(c)->getWidget();
        switch(familyMembers)
        {
        case Family::children:{
            unsigned int num_children = listener.countMoldableChildren();
            for (unsigned int k = 0; k < num_children; k++)
            {
                Moldable* child = listener.child(k);
                new_listeners->push_back(new Listener(child));
            }
            break;}

        case Family::parent:{
            Moldable* parent = (Moldable*) listener.parent();
            new_listeners->push_back(new Listener(parent));
            break;}

        case Family::siblings:{
            Moldable* parent = (Moldable*) listener.parent();
            unsigned int num_siblings = parent->countMoldableChildren();
            for (unsigned int k = 0;  k < num_siblings; k++)
            {
                Moldable* sibling = parent->child(k);
                if (sibling != &listener)
                {
                    new_listeners->push_back(new Listener(sibling));
                }
            }
            break;}
        default:
            new_listeners->push_back(new Listener(&listener));
        }
    }

    listenerMap[&broadcast] = new_listeners;
#ifdef DEBUG // Make sure  that all listeners are actual objects
    for (unsigned int i = 0; i < new_listeners->size(); i++)
    {
    	assert (new_listeners->at(i) != 0);
    }
#endif
    delete current_listeners;
}

void updateListeners(BroadcastMessage& broadcast)
{
    Action::SignalAction direction = broadcast.properties->action;
    Listeners* current_listeners = listenerMap[&broadcast];
    Listeners* new_listeners = new Listeners();

    size_t num_current = current_listeners->size();
    for (size_t c = 0; c < num_current; c++)
    {
        Moldable& listener = current_listeners->at(c)->getWidget();
        switch(direction)
        {

        // Add the parent of each of the current listeners.
        case Action::bubble:{
            Moldable* parent = (Moldable*) listener.parent();
            new_listeners->push_back(new Listener(parent));
            break;}

        // Add the children of each of the current listeners.
        case Action::trickle:{
            int num_children = listener.countMoldableChildren();
            for (int k = 0; k < num_children; k++)
            {
                Moldable* child = listener.child(k);
                new_listeners->push_back(new Listener(child));
            }
            break;}

        default: // Something is very wrong
#ifdef DEBUG
        	assert(1==0); // Force assertion error on debug
#endif
        	return;// Don't do anything more if the direction is unclear.
        }
        listenerMap[&broadcast] = new_listeners;

        // Free the memory the current_listener list is using.
        for (size_t  i = 0; i < num_current; i++)
        {
        	delete current_listeners->at(i);
        }
        delete current_listeners;
    }
#ifdef DEBUG // Make sure  that all listeners are actual objects
    for (unsigned int i = 0; i < new_listeners->size(); i++)
    {
    	assert (new_listeners->at(i) != 0);
    }
#endif
}

void directListeners(BroadcastMessage& broadcast)
{
	mApp;
    namespace Action = Enums::SignalActions;
    BroadcastMessage* bptr 		= &broadcast;
    Listeners* listeners 		= listenerMap[bptr];
    assert(listeners !=0);
    size_t num_listeners = listeners->size();
    Nodes::NodeValue& message 	= broadcast.properties->message.value;
    Action::SignalAction action = broadcast.properties->action;

#ifdef DEBUG
    if (broadcast.broadcaster!=0)
    {
		std::cout << "Parsing Action " << action << " for ";
		std::cout << listeners->size() << " listeners from ";
		std::cout << broadcast.broadcaster->getNode() << std::endl;
		std::cout << "Message: ";
		if (message.getType() == Nodes::string_value) std::cout << message.getString();
		else std::cout << message.getInt();

		std::cout << std::endl;
		std::cout << "Listeners: ";
		for (size_t i = 0; i < listeners->size(); ++i)
		{
			if (&(listeners->at(i)->getWidget()) != NULL)
			{
				std::cout << listeners->at(i)->getWidget().getNode();
			}
			else
			{
				std::cout << listeners->at(i)->getString();
			}
			std::cout << " ";
		}
		std::cout << std::endl;
    }
#endif


	switch(action)
	{
	case Action::set_internal_path:{
		std::string path = message.getString();
		app->setInternalPath(path);
		break;}

	case Action::register_user:{
		handleBoolAction(broadcast, &Actions::register_user);
		break;}

	case Action::change_session:{
		handleBoolAction(broadcast, &Actions::change_session);
		break;}

	case Action::javascript:{
		std::string script = message.getString();
		app->doJavaScript(script);
		break;}

	case Action::email_user:{
		std::string emailmsg = message.getString();
		Actions::EmailPacket pkt;
		std::string URL = app->bookmarkUrl();
		pkt.subject = "New eMail from " + URL;
		pkt.message = emailmsg;
		Actions::email_current_user(&pkt);
		break;}

	case Action::reset_password:{
		std::string uid = app->getSlots()["USERID"];
		if (!Actions::reset_password(uid))
		{
			broadcast.broadcaster->fail().emit();
		}
		else
		{
			broadcast.broadcaster->succeed().emit();
		}
		break;
	}

	/* Change the CSS style class of all listeners to the style
     * given as the broadcast message.
     */
    case Action::set_style:{

        string new_style = message.getString();
        Wt::WString wNewStyle(new_style);

        for (size_t w = 0; w < num_listeners; w++)
        {
            Moldable& widget = listeners->at(w)->getWidget();
            if (widget.allowsAction(Action::set_style))
                widget.setStyleClass(wNewStyle);
        }
        break;}

    case Action::emit:{
    	Actions::emit(*listeners, message.getString());
    	break;}

    /* Change the widget index of the listeners' stacked widgets. */
    case Action::set_index:
    	Actions::set_index(*listeners, message.getInt());
    	break;

    /* Append a new CSS class to the current widget style. */
    case Action::add_class:{
    	for (size_t w = 0; w < num_listeners; w++)
    	{
    		Moldable& widget = listeners->at(w)->getWidget();
    		if (widget.allowsAction(Action::add_class))
    		{
    			Wt::WString addl_style = message.getString();
    			widget.addStyleClass(addl_style);
    		}
    	}
        break;}

    /* Remove a CSS class from the current widget style. */
    case Action::remove_class:{
    	for (size_t w = 0; w < num_listeners; w++)
    	{
    		Moldable& widget = listeners->at(w)->getWidget();
    		if (widget.allowsAction(Action::remove_class))
    		{
    			Wt::WString msg = message.getString();
    			widget.removeStyleClass(msg);
    		}
    	}
    	break;}

    /* Advance the stack's widget index. */
    case Action::increment_index: Actions::increment_index(*listeners);
    	break;

    /* Rewind the stack's widget index. */
    case Action::decrement_index: Actions::decrement_index(*listeners);
    	break;

    /* Adds a new widget to the tree. */
    case Action::add_widget:{
    	for (size_t w = 0; w < num_listeners; w++)
    	{
    		Moldable& widget = listeners->at(w)->getWidget();
    		Nodes::NodeValue v;
    		app->interpreter().giveInput(
    				message.getString(), v);

    		if (widget.allowsAction(Action::add_widget))
			{
    			MoldableTemplate* tpl =
    					MoldableFactory::conceptualize(v.getString());
    			widget.addWidget(MoldableFactory::sculpt(tpl));
			}
    	}
    	break;}

    /* Removes a widget from the tree. */
    /* Note: Does not delete the widget! (?) */
    case Action::remove_child:{
    	for (size_t w = 0; w < num_listeners; w++)
    	{
    		Moldable& widget = listeners->at(w)->getWidget();
    		if (widget.allowsAction(Action::remove_child))
    		{
    			Moldable* child = NULL;
    			Nodes::ReadType msg_type;
    			msg_type = message.getType();
    			if (msg_type == Nodes::string_value)
    			{
    				std::string msg = message.getString();
    				if (app->widgetIsRegistered(msg))
    				{
    					child = app->registeredWidget(msg);
    				}
    			}
    			else
    			{
    				int msg =message.getInt();
    				child = (Moldable*) widget.widget(msg);
    			}
    			if (child != NULL)
    			{
    				widget.requestRemoval(child);
    			}
    		}

    	}
    	break;}

    case Action::clear:{
    	for (size_t w = 0; w < num_listeners; w++)
    	{
    		Moldable& widget = listeners->at(w)->getWidget();
    		if (widget.allowsAction(Action::clear))
    		{
    			widget.clear();
    		}
    	}
    	break;}

    /* 'listeners' here are the policy names and additional field arugments.
     * The message is the data to be stored. If 'listeners' contains only a
     * single argument, the data cannot be given to a hash node.
     */
    case Action::store_value:
    	Actions::store(*listeners, message);
    	break;

    case Action::store_abstract:{
    	for (size_t w = 0; w < num_listeners; w++)
    	{
    		Moldable& widget = listeners->at(w)->getWidget();
    		if (widget.allowsAction(Action::store_abstract))
    		{
    			using namespace Parsers::StyleParser;
    			Nodes::NodeValue v;
    			v.setString(getWidgetProperty(widget.getNode(),"abstract"));
    			Redis::StorageRequest r(message.getString(), v);
    			r.execute();
    			//TODO REPLACE WITH REDIS STORAGE REQUEST!
    		}
    	}
    	break;}

    case Action::slot:{
    	for (size_t w = 0; w < num_listeners; w++)
    	{
    		Moldable& widget = listeners->at(w)->getWidget();
    		if (widget.allowsAction(Action::slot))
    		{
    			std::string slot_ = message.getString();
    			std::string value_ = widget.valueCallback();
    			app->getSlots()[slot_] = value_;
    		}
    	}
    	break;}

    case Action::match:
    case Action::test_text:
    	Actions::test( listeners->at(0)->getWidget(), message) ?
    				broadcast.broadcaster->succeed().emit()
    			: 	broadcast.broadcaster->fail().emit();
    	break;

    case Action::set_text:{
    	for (size_t w = 0; w < num_listeners; w++)
    	{
    		Moldable& widget = listeners->at(w)->getWidget();
    		if (widget.allowsAction(Action::set_text))
			{
    			widget.setValueCallback(message.getString());
			}
    	}
    	break;}

    case Action::reload:{
    	for (size_t w = 0; w < num_listeners; w++)
    	{
    		Moldable& widget = listeners->at(w)->getWidget();
    		MoldableFactory::sculpt(widget.getProperties(), &widget);
    	}
    	break;}

    case Action::STDOUT:{
    	for (size_t w = 0; w < num_listeners; w++)
    	{
    		Moldable& widget = listeners->at(w)->getWidget();
    		std::cout << widget.getNode() << " says: ";
    		std::cout << message.getString() << std::endl;
    	}
    }

    default:return; // Don't do anything unexpected!
    }
#ifdef DEBUG
    std::cout << "DONE. Cleaning Up." << std::endl;
#endif
}


void cleanupBroadcast(BroadcastMessage& broadcast)
{
    /* First, delete all Listener objects associated with the broadcast */
    delete listenerMap[&broadcast];
    listenerMap.erase(&broadcast);
    broadcast.updateAction();
}

} //Namespace ActionCenter
} //Namespace Events
