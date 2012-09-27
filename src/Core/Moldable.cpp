/*
 * Moldable.cpp
 *
 *  Created on: Jun 4, 2012
 *      Author: tom
 */

#include <Core/Moldable.h>
#include <Core/MoldableFactory.h>
#include <Wt/WContainerWidget>
#include <Wt/WStackedWidget>
#include <Wt/WString>

#include <Events/Bindery.h>
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

Moldable::Moldable(MoldableTemplate* tpl)
: Wt::WContainerWidget ()
    ,children()
    ,__style_changed(this)
    ,__index_changed(this)
    ,__failed_test(this)
    ,__succeeded_test(this)
{
	__tmpl = tpl;
#ifdef DEBUG
    __NODE_NAME = __tmpl->node.c_str();
    std::cout << "CREATING " << __NODE_NAME << std::endl;
#endif

    // Not all widgets will have a bindery. This may remain null.
    bindery =0;
}

Moldable::~Moldable()
{
	if (bindery !=0) delete bindery;
	delete __tmpl;
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

		if (__tmpl->flags & Enums::WidgetTypes::has_events)
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
	MoldableTemplate* __tmpl = MoldableFactory::conceptualize(nodeName);
	Moldable* newGoo = MoldableFactory::sculpt(__tmpl);
	addWidget(newGoo);

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
