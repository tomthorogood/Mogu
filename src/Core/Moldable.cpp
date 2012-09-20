/*
 * Moldable.cpp
 *
 *  Created on: Jun 4, 2012
 *      Author: tom
 */

#include <Core/Moldable.h>
#include <Core/Dynamic.h>
#include <Wt/WStackedWidget>
#include <Wt/WString>

#include <Events/Bindery.h>
#include <Core/Sculptory.h>
#include <Parsers/StyleParser.h>
#include <Parsers/Parsers.h>
#include <Types/NodeValue.h>
#include <Static.h>
#include <Mogu.h>
#include <hash.h>

#include <iostream>

#include <Exceptions.h>

namespace Goo
{
using std::string;
using namespace Enums::WidgetTypes;
using namespace Enums::BitMasks;
using Nodes::NodeValue;
using std::cout;
using std::endl;

GooVariables::GooVariables() : children()
{
    flags		        =0;
    actionBlocking      =0;
    type				=0;
    dynamic				=false;

    content         = "";
    location        = "";
    source          = "";
}

Moldable::Moldable(
        const string& constructorNode,
        Wt::WContainerWidget* parent
)
: Wt::WContainerWidget (parent)
    ,children()
    ,__style_changed(this)
    ,__index_changed(this)
    ,__failed_test(this)
    ,__succeeded_test(this)
{
	__node = constructorNode;

#ifdef DEBUG
    __NODE_NAME = constructorNode.c_str();
    std::cout << "CREATING " << __NODE_NAME << std::endl;
#endif

    // Not all widgets will have a bindery. This may remain null.
    bindery =0;
    baseVariables = new GooVariables();

    /* Allow the sculptory to determine how this widget should look once it is
     * loaded.
     */
    conceptualize(this);
}

Moldable::~Moldable()
{
	if (bindery !=0) delete bindery;
	delete baseVariables;
	std::map<States,NodeValue*>::iterator cache_iter = __state_cache.begin();
	while (cache_iter != __state_cache.end())
	{
		delete cache_iter->second;
		++cache_iter;
	}
}


void
Moldable::load()
{
    /* Don't create content until the widget is loaded and visible. */
    if (!loaded() || __reload)
    {

#ifdef DEBUG
    	std::cout << "Loading: " << __NODE_NAME << std::endl;
#endif

		Wt::WContainerWidget::load();

		//Use the properties to draw the widget on the user's screen.
		mold(this);

		if (baseVariables->flags & Enums::WidgetTypes::has_events)
		{
			do_if_has_events();
		}
		__reload = false; //Don't allow this to be reloaded accidentally.
    }
}

std::string Moldable::getMappedValue(std::string key)
{
	return Parsers::StyleParser::getWidgetProperty(__node, key.c_str());
}

void Moldable::__validate()
{
	Wt::WLineEdit* input = (Wt::WLineEdit*) widget(0);
	Wt::WValidator::State result = input->validate();
	if (result == Wt::WValidator::Valid)
	{
		__succeeded_test.emit();
		return;
	}
	__failed_test.emit();
} // validate

void Moldable::addGoo (const string& nodeName)
{
	Moldable* newGoo =0;

	/* If the new widget is dynamic, we must ensure the
	 * integrity of the user session
	 */
	if (Parsers::StyleParser::widgetIsDynamic(nodeName))
	{
		/* We retrieve the auth token for the session to store it with the widget
		 * ensuring that the widget can only be written or displayed as long as
		 * the auth token remains valid.
		 */
		std::string token = Application::requestAuthToken(
				Application::requestSessionID(
				Application::mogu()->sessionId()));
		token = Hash::toHash(token);
		newGoo = new Dynamic(
				Application::encrypt(token),
				nodeName);
	}
	// If we haven't set newGoo yet, it must be a standard Moldable widget.
	if (newGoo ==0)
	{
		newGoo = new Moldable(nodeName);
	}

	/* If we have a stacked widget, we must add the new child to the stack,
	 * not to the base widget.
	 */
	if ( (baseVariables->type & WIDGET_HO_BITS) == Enums::WidgetTypes::stack)
    {
        Wt::WStackedWidget* stack =
                (Wt::WStackedWidget*) widget(0);
        stack->addWidget(newGoo);
    }

	// Otherwise, add the widget as normal.
    else
    {
        addWidget(newGoo);
    }

    /* Because the children vector receives a pointer to the newGoo widget,
     * it doesn't really matter whether or not it is contained directly as
     * a child of this widget, or as a stacked container child, because it
     * can be interacted with just the same from external widgets requesting
     * access to their content. Isn't that special?
     */
    children.add(newGoo);
    children.trim();

#ifdef DEBUG
    for (unsigned int i = 0; i < children.size(); i++)
    {
    	assert (children.at(i) != 0);
    }
#endif
} // addGoo()

Nodes::NodeValue* Moldable::getState(Enums::WidgetTypes::States state)
{
	using namespace Enums::WidgetTypes;
	Nodes::NodeValue* val = new Nodes::NodeValue();
	switch(state)
	{
	case num_children:{
		int n = countMoldableChildren();
		val->setInt(n);
		break;}
	case current_index:{
		Wt::WStackedWidget* stack = (Wt::WStackedWidget*) widget(0);
		int n = stack->currentIndex();
		val->setInt(n);
		break;}
	case is_hidden:{
		bool v = isHidden();
		val->setInt( (bool) v);
		break;}
	default:
		val->setInt(0);
	}
	__state_cache[state] = val;
	return val;
}


}// namespace Goo
