/*
 * MoldableActionCenter.cpp
 *
 *  Created on: Jul 12, 2012
 *      Author: tom
 */


#include <Events/MoldableActionCenter.h>
#include <Events/BroadcastMessage.h>
#include <Events/EventPreprocessor.h>
#include <Parsers/Parsers.h>
#include <Parsers/StyleParser.h>
#include <Parsers/NodeValueParser.h>
#include <Wt/WStackedWidget>
#include <Wt/WText>
#include <Core/Moldable.h>
#include <Core/MoldableTemplate.h>
#include <Core/MoldableFactory.h>
#include <Static.h>
#include <Mogu.h>
#include <Wt/WString>
#include <Sessions/Submission.h>
#include <stdlib.h>
#include <signal.h>

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

inline bool listenerIsRegistered(
		Family::_Family& fenum, BroadcastMessage& msg)
{
	fenum = msg.properties->listener.f_listener;
	return
			msg.properties->listener.r_listener != EMPTY;
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

void submitBroadcast(BroadcastMessage& broadcast)
{
	mApp;
    Family::_Family f_listener;
    bool registryListener = listenerIsRegistered(f_listener, broadcast);


    /* If this is the first time we're transmitting this broadcast,
     * determine who should be listening to it.
     */
    ListenerMap::iterator iter = listenerMap.find(&broadcast);
    if (iter == listenerMap.end())
    {
        Listeners* listener = new Listeners();
        /* If the listener is a registered listener, we can get a pointer
         * to the listener directly by name.
         */
        if (registryListener)
        {
        	std::string name = broadcast.properties->listener.r_listener;
        	if (app->widgetIsRegistered(name))
        	{
        		listener->push_back(app->registeredWidget(name));
        	}
        }

        else // Otherwise, provide a default value of the broadcaster.
        {
        	listener->push_back(broadcast.broadcaster);
        }

        listenerMap[&broadcast] = listener;
    }
#ifdef DEBUG
    std::string action_str;
    if (broadcast.properties->action == Enums::SignalActions::trickle)
    	action_str = "trickle";
    else if
    	(broadcast.properties->action == Enums::SignalActions::bubble)
    	action_str = "bubble";
    else action_str = "ERROR";
    if (broadcast.properties->degradation > 0)
    {
    	std::cout << "Degradation: " << broadcast.properties->degradation << " ";
    	std::cout << "| Action: " << action_str << std::endl;
    }
#endif
    /* If this broadcast is getting repeated, determine who the next set
     * of listeners will be, and then start over
     */
    int degradation = broadcast.properties->degradation;

    while (--degradation>=0)
    {
    	updateListeners(broadcast);
    }
    broadcast.updateAction();

    if (broadcast.properties->action == Action::rebroadcast)
    {
    	if (!registryListener && (f_listener != Family::application))
    	{
    		getNuclearFamily(broadcast);
    	}
    	string nodeName =
    			"events."+broadcast.properties->message.value.getString();
    	Listeners* listeners = listenerMap[&broadcast];
    	size_t num_listeners = listeners->size();
    	for (size_t listener = 0; listener < num_listeners; ++listener)
    	{
    		Moldable* __listener = listeners->at(listener);
    		EventPreprocessor preproc(nodeName);
    		BroadcastMessage newmsg(__listener, &preproc);
    		submitBroadcast(newmsg);
    	}

    	cleanupBroadcast(broadcast);
        return; // Avoid side effects with recursion!
    }

    /* If this is a nuclear event, we lastly have to do one more
     * calculation of listeners.
     */
    if (!registryListener && (f_listener != Family::application))
    {
        getNuclearFamily(broadcast);
    }

    directListeners(broadcast);
    cleanupBroadcast(broadcast);
}

void getNuclearFamily(BroadcastMessage& broadcast)
{
    namespace Family = Enums::Family;
    Family::_Family familyMembers = broadcast.properties->listener.f_listener;
    Listeners* current_listeners = listenerMap[&broadcast];
    Listeners* new_listeners = new Listeners();

    unsigned int num_current = current_listeners->size();
#ifdef DEBUG
    assert(num_current != 0);
    for (unsigned int i = 0; i < num_current; i++)
    {
    	assert (current_listeners->at(i) != 0);
    }
#endif
    for (unsigned int c = 0; c < num_current; c++)
    {
        Moldable* listener = current_listeners->at(c);
        switch(familyMembers)
        {
        case Family::children:{
            unsigned int num_children = listener->countMoldableChildren();
            for (unsigned int k = 0; k < num_children; k++)
            {
                Moldable* child = listener->child(k);
                new_listeners->push_back(child);
            }
            break;}

        case Family::parent:{
            Moldable* parent = (Moldable*) listener->parent();
            new_listeners->push_back(parent);
            break;}

        case Family::siblings:{
            Moldable* parent = (Moldable*) listener->parent();
            unsigned int num_siblings = parent->countMoldableChildren();
            for (unsigned int k = 0;  k < num_siblings; k++)
            {
                Moldable* sibling = parent->child(k);
                if (sibling != listener)
                {
                    new_listeners->push_back(sibling);
                }
            }
            break;}
        default:
            new_listeners->push_back(listener);
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
        Moldable* listener = current_listeners->at(c);
        switch(direction)
        {

        // Add the parent of each of the current listeners.
        case Action::bubble:{
            Moldable* parent = (Moldable*) listener->parent();
            new_listeners->push_back(parent);
            break;}

        // Add the children of each of the current listeners.
        case Action::trickle:{
            int num_children = listener->countMoldableChildren();
            for (int k = 0; k < num_children; k++)
            {
                Moldable* child = listener->child(k);
                new_listeners->push_back(child);
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
    Nodes::NodeValue& message 	= broadcast.properties->message.value;

    Action::SignalAction action = broadcast.properties->action;
    Family::_Family f_listener;
    bool registryListener = listenerIsRegistered(f_listener, broadcast);


#ifdef DEBUG // Make sure  that all listeners are actual objects
    if (broadcast.broadcaster!=0)
    {
		std::cout << "Parsing Action " << action << " for ";
		std::cout << listeners->size() << " listeners from ";
		std::cout << broadcast.broadcaster->getNode() << std::endl;
		std::cout << "Listeners: ";
		for (size_t i = 0; i < listeners->size(); ++i)
		{
			if (i != 0) std::cout << ", ";
			std::cout << listeners->at(i)->getNode();
		}
		std::cout << std::endl;
		std::cout << "Message: ";
		if (message.getType() == Nodes::string_value) std::cout << message.getString();
		else std::cout << message.getInt();
		std::cout << std::endl;
    }
#endif

    if (!registryListener && (f_listener == Enums::Family::application))
    {
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
        	std::string msg = message.getString();
        	std::string uid = Application::retrieveSlot(
        			"USERID", app->sessionId());


        	if (!Actions::reset_password(uid))
        	{
        		broadcast.broadcaster->fail().emit();
        	}
        	else
        	{
        		broadcast.broadcaster->succeed().emit();
        	}
        }

    	default:
    		return; // Don't do anything unexpected to application state.
    	}
    	return; // Avoid side effects with non-existent Moldable listener.
    }

    int num_listeners = listeners->size();
    switch(action)
    {

    /* Change the CSS style class of all listeners to the style
     * given as the broadcast message.
     */
    case Action::set_style:{
        string new_style = message.getString();
        Wt::WString wNewStyle(new_style);

        for (int w = 0; w < num_listeners; w++)
        {
            Moldable* widget = listeners->at(w);
            if (widget->allowsAction(Action::set_style))
            {
                widget->setStyleClass(wNewStyle);
            }
        }
        break;}

    /* Change the widget index of the listeners' stacked widgets. */
    case Action::set_index:
    	Actions::set_index(listeners, message.getInt());
    	break;

    /* Append a new CSS class to the current widget style. */
    case Action::add_class:{
    	for (int w = 0; w < num_listeners; w++)
    	{
    		Moldable* widget = listeners->at(w);
    		if (widget->allowsAction(Action::add_class))
    		{
    			std::string current_style = widget->styleClass().toUTF8();
    			std::string addl_style = message.getString();
    			current_style.append(" "+addl_style);
    			Wt::WString wNewStyle(current_style);
    			widget->setStyleClass(wNewStyle);
    		}
    	}
        break;}

    /* Remove a CSS class from the current widget style. */
    case Action::remove_class:{
    	for (int w = 0; w < num_listeners; w++)
    	{
    		Moldable* widget = listeners->at(w);
    		if (widget->allowsAction(Action::remove_class))
    		{
    			uint8_t start = 0;
    			uint8_t end = 1;
    			std::string current_style = widget->styleClass().toUTF8();
    			std::string remove_style = message.getString();
    			std::string new_style;
    			int slice[2];
    			slice[start] = current_style.find(remove_style);
    			slice[end] = slice[start] + remove_style.length();

    			new_style = current_style.substr(0,slice[start]);
    			new_style
					.append(current_style
						.substr(
							slice[end],
							(current_style.length()-slice[end])));
    			Wt::WString wNewStyle(new_style);
    			widget->setStyleClass(wNewStyle);
    		}
    	}
    	break;}

    /* Advance the stack's widget index. */
    case Action::increment_index: Actions::increment_index(listeners);
    	break;

    /* Rewind the stack's widget index. */
    case Action::decrement_index: Actions::decrement_index(listeners);
    	break;

    /* Adds a new widget to the tree. */
    case Action::add_widget:{
    	for (int w = 0; w < num_listeners; w++)
    	{
    		Moldable* widget = listeners->at(w);
    		Nodes::NodeValue v;
    		Parsers::NodeValueParser p(
    				message.getString(), &v);
    		if (widget->allowsAction(Action::add_widget))
			{
    			MoldableTemplate* tpl =
    					MoldableFactory::conceptualize(v.getString());
    			widget->addWidget(MoldableFactory::sculpt(tpl));
			}
    	}
    	break;}

    /* Removes a widget from the tree. */
    /* Note: Does not delete the widget! (?) */
    case Action::remove_child:{
    	for (int w = 0; w < num_listeners; w++)
    	{
    		Moldable* widget = listeners->at(w);
    		if (widget->allowsAction(Action::remove_child))
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
    				child = (Moldable*) widget->widget(msg);
    			}
    			if (child != NULL)
    			{
    				widget->requestRemoval(child);
    			}
    		}

    	}
    	break;}

    case Action::clear:{
    	for (int w = 0; w < num_listeners; w++)
    	{
    		Moldable* widget = listeners->at(w);
    		if (widget->allowsAction(Action::clear))
    		{
    			widget->clear();
    		}
    	}
    	break;}


    case Action::store_value:{
    	for (int w = 0; w < num_listeners; w++)
    	{
    		Moldable* widget = (Moldable*) listeners->at(w);
    		if (widget->allowsAction(Action::store_value))
    		{
    			std::string val = widget->valueCallback();
    			std::string snode 	= message.getString();
    			if (widget->allowsAction(Action::store_value))
    			{
    				Sessions::SubmissionHandler::absorb(val, snode);
    			}
    		}
    	}
    	break;}

    case Action::store_abstract:{
    	for (int w = 0; w < num_listeners; w++)
    	{
    		Moldable* widget = (Moldable*) listeners->at(w);
    		if (widget->allowsAction(Action::store_abstract))
    		{
    			using namespace Parsers::StyleParser;
    			std::string abstract = getWidgetProperty(
    					widget->getNode(), "abstract");
    			Sessions::SubmissionHandler::absorb(
    					abstract, message.getString());
    		}
    	}
    	break;}

    case Action::slot:{
    	for (int w = 0; w < num_listeners; w++)
    	{
    		Moldable* widget = (Moldable*) listeners->at(w);
    		if (widget->allowsAction(Action::slot))
    		{
    			std::string slot_ = message.getString();
    			std::string value_ = widget->valueCallback();
    			Application::slotStorage(slot_, value_);
    		}
    	}
    	break;}

    case Action::match:
    case Action::test_text:
    	Actions::test(*(listeners->at(0)), message) ?
    				broadcast.broadcaster->succeed().emit()
    			: 	broadcast.broadcaster->fail().emit();
    	break;

    case Action::set_text:{
    	for (int w = 0; w < num_listeners; w++)
    	{
    		Moldable* widget = listeners->at(w);
    		if (widget->allowsAction(Action::set_text))
			{
    			widget->setValueCallback(message.getString());
			}
    	}
    	break;}

    case Action::reload:{
    	for (int w = 0; w < num_listeners; w++)
    	{
    		Moldable* widget = (Moldable*) listeners->at(w);
    		MoldableFactory::sculpt(widget->getProperties(), widget);
    	}
    	break;}

    case Action::STDOUT:{
    	for (int w = 0; w < num_listeners; w++)
    	{
    		Moldable* widget = (Moldable*) listeners->at(w);
    		std::cout << widget->getNode() << " says: ";
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
