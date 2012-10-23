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
#include <Parsers/MoguScript_Tokenizer.h>
#include <Types/NodeValue.h>
#include <Static.h>
#include <Mogu.h>
#include <hash.h>

#include <iostream>

namespace Goo
{
using std::string;
using namespace Enums::WidgetTypes;
using namespace Parsers::StyleParser;
using Nodes::NodeValue;
using std::cout;
using std::endl;

Moldable::Moldable() : Wt::WContainerWidget()
{
	__reload = true;
	__setvalue_callback =0;
	__value_callback =0;
	__tmpl =0;
	bindery =0;
}

Moldable::Moldable(MoldableTemplate* tpl)
: Wt::WContainerWidget ()
    ,mchildren()
    ,__style_changed(this)
    ,__index_changed(this)
    ,__failed_test(this)
    ,__succeeded_test(this)
    ,__hidden_changed(this)
    ,__onload(this)
{
	__tmpl = tpl;
#ifdef DEBUG
    std::cout << "CREATING " << __tmpl->node << std::endl;
#endif

    // Not all widgets will have a bindery. This may remain null.
    bindery =0;
    __reload = true;
    __setvalue_callback =0;
    __value_callback =0;
}

Moldable::~Moldable()
{
#ifdef DEBUG
	cout << "Deleting " << __tmpl->node << endl;
#endif
	if (bindery !=0) delete bindery;
	__tmpl->disconnect();
	if (__tmpl->deleteable) delete __tmpl;
}


void
Moldable::load()
{
    /* Don't create content until the widget is loaded and visible. */
    if (!loaded() || __reload)
    {

#ifdef DEBUG
    	std::cout << "Loading: " << __tmpl->node << std::endl;
#endif

		Wt::WContainerWidget::load();

		if (__tmpl->flags & Enums::WidgetTypes::has_events)
		{
            bindery = new Events::EventBindery(this);
		}
		
        __reload = false; //Don't allow this to be reloaded accidentally.
		onLoad().emit(); // Activate any post-load callbacks.
    }
}

void Moldable::setHidden(
		bool hidden, const Wt::WAnimation& animation)
{
	Wt::WContainerWidget::setHidden(hidden,animation);
	__hidden_changed.emit(isHidden());
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

void Moldable::getState(Enums::WidgetTypes::States state,
		Nodes::NodeValue& val)
{
	using namespace Enums::WidgetTypes;
	switch(state)
	{
	case num_children:{
		int n = countMoldableChildren();
		val.setInt(n);
		break;}
	case current_index:{
		Wt::WStackedWidget* stack = (Wt::WStackedWidget*) widget(0);
		int n = stack->currentIndex();
		val.setInt(n);
		break;}
	case is_hidden:{
		bool v = isHidden();
		val.setInt( (bool) v);
		break;}
	case value:{
		val.setString(valueCallback());
		break;}
	case currentstyle:{
		val.setString(styleClass().toUTF8());
		break;}
	default:
		val.setInt(0);
	}
}

bool Moldable::allowsAction(Enums::SignalActions::SignalAction action)
{
	if (!widgetHasField(getNode(), "block")) return true;
	std::string action_blocks = getWidgetField(getNode(), "block");
	Parsers::MoguScript_Tokenizer t(action_blocks);
	std::string b = t.next();
	Nodes::NodeValue v;
	while (b != EMPTY)
	{
		Parsers::NodeValueParser p(
				b, v,
				this,
				&Parsers::enum_callback <Parsers::SignalActionParser>);
		Enums::SignalActions::SignalAction block = (Enums::SignalActions::SignalAction)
				v.getInt();
		if (block == action) return false;
		b = t.next();
	}
	return true;

}


}// namespace Goo
