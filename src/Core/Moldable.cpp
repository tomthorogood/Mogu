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

#include <Parsers/Parsers.h>
#include <Parsers/StyleParser.h>
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
: Wt::WContainerWidget (parent),
    children(),
    __style_changed(this),
    __failed_test(this),
    __succeeded_test(this)
{
    nodes.add(constructorNode);
#ifdef DEBUG
    __NODE_NAME = constructorNode.c_str();
    std::cout << "CREATING " << __NODE_NAME << std::endl;
#endif
    bindery =0;
    baseVariables = new GooVariables();
    conceptualize(this);
}

Moldable::~Moldable()
{
    if (bindery != 0) delete bindery;
    if (baseVariables->flags & Enums::WidgetTypes::is_named)
    {
    	Application::mogu()->deregisterWidget(
    			Parsers::StyleParser::getWidgetName(this)
    	);
    }
    delete baseVariables;
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
		mold(this);
		if (baseVariables->flags & Enums::WidgetTypes::has_events)
		{
			do_if_has_events();
		}
		__reload = false;
    }
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
}

void Moldable::addGoo (const string& nodeName)
{
	Moldable* newGoo =0;
	if (Parsers::StyleParser::widgetIsDynamic(nodeName))
	{
		std::string token = Application::requestAuthToken(
				Application::requestSessionID(
				Application::mogu()->sessionId()));
		token = Hash::toHash(token);
		newGoo = new Dynamic(
				Application::encrypt(token),
				nodeName);
	}
	if (newGoo ==0)
	{
		newGoo = new Moldable(nodeName);
	}

	if ( (baseVariables->type & WIDGET_HO_BITS) == Enums::WidgetTypes::stack)
    {
        Wt::WStackedWidget* stack =
                (Wt::WStackedWidget*) widget(0);
        stack->addWidget(newGoo);
    }

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
}

void Moldable::setStyleClass(const Wt::WString& style)
{
    Wt::WContainerWidget::setStyleClass(style);
    __style_changed.emit();
}

}//namespace Goo
