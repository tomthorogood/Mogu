/*
 * MoldableActionCenter.cpp
 *
 *  Created on: Jul 12, 2012
 *      Author: tom
 */


#include <Events/MoldableActionCenter.h>
#include <Events/EventNodeExtractor.h>
#include <Events/EventNodeProcessor.h>
#include <Events/NodeConfiguration.h>
#include <Events/BroadcastMessage.h>
#include <Parsers/Parsers.h>
#include <Wt/WStackedWidget>
#include <Core/Moldable.h>
#include <Static.h>
#include <Mogu.h>
#include <Wt/WString>

namespace Events
{
namespace ActionCenter
{

using std::string;
namespace Action = Enums::SignalActions;

using Goo::Moldable;

namespace {
    ListenerMap listenerMap;
    int SIGNALRANGE[2] = {198,233};
}

void submitBroadcast(BroadcastMessage* broadcast)
{
    namespace TypeBits = Enums::SignalTypes;
    unsigned char signalType = broadcast->getSignalType();

    // Verify the integrity of this broadcast.
    if (signalType < SIGNALRANGE[0] || signalType > SIGNALRANGE[1])
    {
        //@TODO: Throw Error!
    }

    /* If this is the first time we're transmitting this broadcast,
     * determine who should be listening to it.
     */
    ListenerMap::iterator iter = listenerMap.find(broadcast);
    if (iter == listenerMap.end())
    {
        Listeners* listener = new Listeners();
        if (signalType & TypeBits::registered_listener)
        {
        	std::string listener_name =
        			broadcast->getListenerName();
        	Mogu* app = Application::mogu();
        	if (app->widgetIsRegistered(listener_name))
        	{
        		listener->add(app->registeredWidget(listener_name));
        	}
        }
        else
        {
        	listener->add(broadcast->getBroadcaster());
        }

        listener->trim();
        listenerMap[broadcast] = listener;
    }

    /* If this broadcast is getting repeated, determine who the next set
     * of listeners will be, and then start over
     */
    int degradation = broadcast->degrade();
    if (degradation>0)
    {
        updateListeners(broadcast);
        submitBroadcast(broadcast);
        return; // Avoid side effects with recursion!
    }
    /* If the broadcast was previously repeated, but is on its final stop
     * we'll upgrade the action to be the nextAction if applicable,
     * and start over.
     */
    if (degradation == 0 )
    {
        broadcast->upgradeAction();
        if (
        		(signalType & TypeBits::pointer) &&
        		!(signalType & TypeBits::registered_listener))
        {
            updateListeners(broadcast);
        }
        submitBroadcast(broadcast);
        return; // Avoid side effects with recursion!
    }

    if (signalType & TypeBits::pointer)
    {
        if (signalType & TypeBits::specific_listeners)
        {
            getNuclearFamily(broadcast);
        }

        string nodeName = broadcast->getMessage()->getString();
        Listeners* listeners = listenerMap[broadcast];
        unsigned int num_listeners = listeners->size();
        for (unsigned int listener =0; listener < num_listeners; listener++)
        {
            Moldable* _listener = listeners->at(listener);
            BroadcastMessage* newMessage =
                generateNewBroadcast(_listener, nodeName);
            submitBroadcast(newMessage);
        }

        cleanupBroadcast(broadcast);
        return; // Avoid side effects with recursion!
    }
    /* If this is a nuclear event, we lastly have to do one more
     * calculation of listeners.
     */
    if ( (signalType & TypeBits::specific_listeners)
    		&& broadcast->getListenerType() != Enums::Family::application)
    {
        getNuclearFamily(broadcast);
    }

    directListeners(broadcast);
    cleanupBroadcast(broadcast);
}

void getNuclearFamily(BroadcastMessage* broadcast)
{
    namespace Family = Enums::Family;
    Family::_Family familyMembers = broadcast->getListenerType();
    Listeners* current_listeners = listenerMap[broadcast];
    Listeners* new_listeners = new Listeners();

    unsigned int num_current = current_listeners->size();
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
                new_listeners->add(child);
            }
            break;}

        case Family::parent:{
            Moldable* parent = (Moldable*) listener->parent();
            new_listeners->add(parent);
            break;}

        case Family::siblings:{
            Moldable* parent = (Moldable*) listener->parent();
            unsigned int num_siblings = parent->countMoldableChildren();
            for (unsigned int k = 0;  k < num_siblings; k++)
            {
                Moldable* sibling = parent->child(k);
                if (sibling != listener)
                {
                    new_listeners->add(sibling);
                }
            }
            break;}
        default:
            new_listeners->add(listener);
        }
    }
    new_listeners->trim();
    listenerMap[broadcast] = new_listeners;
    delete current_listeners;
}

void updateListeners(BroadcastMessage* broadcast)
{
    Action::SignalAction direction = broadcast->getAction();
    Listeners* current_listeners = listenerMap[broadcast];
    Listeners* new_listeners = new Listeners();

    unsigned int num_current = current_listeners->size();
    for (unsigned int c = 0; c < num_current; c++)
    {
        Moldable* listener = current_listeners->at(c);
        switch(direction)
        {

        // Add the parent of each of the current listeners.
        case Action::bubble:{
            Moldable* parent = (Moldable*) listener->parent();
            new_listeners->add(parent);
            break;}

        // Add the children of each of the current listeners.
        case Action::trickle:{
            int num_children = listener->countMoldableChildren();
            for (int k = 0; k < num_children; k++)
            {
                Moldable* child = listener->child(k);
                new_listeners->add(child);
            }
            break;}

        //Something is wrong.
        default: return;// Don't do anything more if the direction is unclear.
        }
        new_listeners->trim();
        listenerMap[broadcast] = new_listeners;

        // Free the memory the current_listener list is using.
        delete current_listeners;
    }
}

void directListeners(BroadcastMessage* broadcast)
{
    namespace Action = Enums::SignalActions;
    Listeners* listeners = listenerMap[broadcast];
    if (broadcast->getListenerType() == Enums::Family::application)
    {
    	if (broadcast->getAction() == Action::set_internal_path)
    	{
    		std::string path = broadcast->getMessage()->getString();
    		Application::mogu()->setInternalPath(path);
    	}
    	return; // Avoid side effects with non-existent Moldable listeners
    }

    int num_listeners = listeners->size();
    switch(broadcast->getAction())
    {

    /* Change the CSS style class of all listeners to the style
     * given as the broadcast message.
     */
    case Action::set_style:{
        string new_style = broadcast->getMessage()->getString();
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
    case Action::set_index:{
        int new_index = broadcast->getMessage()->getInt();

        for (int w = 0; w < num_listeners; w++)
        {
            Moldable* widget = listeners->at(w);
            if (widget->allowsAction(Action::set_index))
            {
                Wt::WStackedWidget* stack =
                        (Wt::WStackedWidget*) widget->widget(0);
                stack->setCurrentIndex(new_index);
            }
        }
        break;}

    /* Append a new CSS class to the current widget style. */
    case Action::add_class:{
    	for (int w = 0; w < num_listeners; w++)
    	{
    		Moldable* widget = listeners->at(w);
    		if (widget->allowsAction(Action::add_class))
    		{
    			std::string current_style = widget->styleClass().toUTF8();
    			std::string addl_style = broadcast->getMessage()->getString();
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
    			std::string remove_style = broadcast->getMessage()->getString();
    			std::string new_style;
    			int slice[2];
    			slice[start] = current_style.find(remove_style);
    			slice[end] = slice[start] + remove_style.length();

    			new_style = current_style.substr(0,slice[start]);
    			new_style
    					.append(current_style
    							.substr(
    									slice[end],
    									(current_style.length()-slice[end])
    					));
    			Wt::WString wNewStyle(new_style);
    			widget->setStyleClass(wNewStyle);
    		}
    	}
    	break;}

    /* Advance the stack's widget index. */
    case Action::increment_index:{
    	for (int w = 0; w < num_listeners; w++)
    	{
    		Moldable* widget = listeners->at(w);
    		if (widget->allowsAction(Action::increment_index))
    		{
    			Wt::WStackedWidget* stack = (Wt::WStackedWidget*)
    					widget->widget(0);
    			int current_index = stack->currentIndex();
    			current_index++;
    			stack->setCurrentIndex(current_index);
    		}
    	}
    	break;}

    /* Rewind the stack's widget index. */
    case Action::decrement_index:{
    	for (int w = 0; w < num_listeners; w++)
    	{
    		Moldable* widget = listeners->at(w);
    		if (widget->allowsAction(Action::decrement_index))
    		{
    			Wt::WStackedWidget* stack = (Wt::WStackedWidget*)
    					widget->widget(0);
    			int current_index = stack->currentIndex();
    			current_index--;
    			stack->setCurrentIndex(current_index);
    		}
    	}
    	break;}

    /* Adds a new widget to the tree. */
    case Action::add_widget:{
    	for (int w = 0; w < num_listeners; w++)
    	{
    		Moldable* widget = listeners->at(w);
    		if (widget->allowsAction(Action::add_widget))
			{
    			std::string constructorNode =
    					broadcast->getMessage()->getString();
    			widget->addGoo(constructorNode);
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
    			msg_type = broadcast->getMessageType();
    			if (msg_type == Nodes::string_value)
    			{
    				Mogu* app = Application::mogu();
    				std::string msg = broadcast->getMessage()->getString();
    				if (app->widgetIsRegistered(msg))
    				{
    					child = app->registeredWidget(msg);
    				}
    			}
    			else
    			{
    				int msg = broadcast->getMessage()->getInt();
    				child = (Moldable*) widget->widget(msg);
    			}
    			if (child != NULL)
    			{
    				widget->requestRemoval(child);
    			}
    		}

    	}
    	break;}

    /* Removes all children from the tree. */
    case Action::clear:{
    	for (int w = 0; w < num_listeners; w++)
    	{
    		Moldable* widget = listeners->at(w);
    		if (widget->allowsAction(Action::clear))
    		{
    			widget->clear();
    		}
    	break;}
    }

    default:return; // Don't do anything unexpected!
    }
}

BroadcastMessage* generateNewBroadcast(
        Moldable* broadcaster, EventNodeExtractor& extract,
        EventNodeProcessor* processor)
{

    namespace Signal = Enums::SignalTypes;
    namespace SignalBits = Enums::SignalTypes;
    namespace Field = Enums::Labels;

    string signal_type_str = extract.getValue(Field::signal);
    string action_str = extract.getValue(Field::action);
    string message_str = extract.getValue(Field::message);

    /*The signal type will always be an integral value. For now. */
    processor->set(Field::signal, signal_type_str);

    /*The action type will always be an enumerated value.*/
    processor->set(Field::action, action_str, broadcaster,
            &Parsers::enum_callback <Parsers::SignalActionParser>);

    /*The message type can be anything EXCEPT an enumerated value*/
    processor->set(Field::message, message_str);

    unsigned char signal_type = (unsigned char)
            processor->getValue(Field::signal)->getInt();


    if (signal_type & SignalBits::signal_repeats)
    {
        string degradation_str = extract.getValue(Field::degradation);
        processor->set(Field::degradation, degradation_str);
    }
    if (signal_type & SignalBits::transforms)
    {
        string next_action_str = extract.getValue(Field::nextAction);
        processor->set(Field::nextAction, next_action_str, broadcaster,
                Parsers::enum_callback <Parsers::SignalActionParser>);
    }

    if (signal_type & SignalBits::registered_listener)
    {
    	string listener_str = extract.getValue(Field::listeners);
    	processor->set(Field::listeners,listener_str);
    }
    else if (signal_type & SignalBits::specific_listeners)
    {
        string listeners_str = extract.getValue(Field::listeners);
        processor->set(Field::listeners, listeners_str, broadcaster,
                Parsers::enum_callback <Parsers::FamilyMemberParser>);
    }

    BroadcastMessage* newMessage =
            new BroadcastMessage(broadcaster, processor);

    return newMessage;
}

BroadcastMessage*  generateNewBroadcast(
        Moldable* broadcaster, EventNodeExtractor& extract)
{
    EventNodeProcessor* processor = new Events::EventNodeProcessor();
    return generateNewBroadcast(broadcaster, extract, processor);
}

BroadcastMessage* generateNewBroadcast(
        Moldable* broadcaster, string& nodeName)
{
    EventNodeExtractor extract(nodeName);
    return generateNewBroadcast(broadcaster, extract);
}

void cleanupBroadcast(BroadcastMessage* broadcast)
{
    listenerMap.erase(broadcast);
    delete broadcast;
}

} //Namespace ActionCenter
} //Namespace Events
