/*
 * Sulptory.cpp
 *
 *  Created on: Oct 1, 2012
 *      Author: tom
 */

#include <Core/MoldableTemplate.h>
#include <Core/Moldable.h>
#include <Core/FactoryStations/Sculptory.h>
#include <Wt/WStackedWidget>
#include <Mogu.h>

namespace Goo{
namespace MoldableFactory{
namespace Sculptory{

using namespace Parsers::StyleParser;
using namespace Application;
using namespace Enums::WidgetTypes;
using namespace Enums::SignalTypes;

void __sculpt_stack(MoldableTemplate* __tmpl, Moldable* m)
{
	Wt::WStackedWidget* stack = new Wt::WStackedWidget();
	if (__tmpl->style != EMPTY)
	{
		setStyle(__tmpl->style,stack);
	}
	if (__tmpl->flags & has_animation)
	{
		Wt::WAnimation::AnimationEffect e = getWidgetAnimation(__tmpl->node);
		Wt::WAnimation transition(e);
		stack->setTransitionAnimation(transition,true);
	}
	if (__tmpl->num_children > 0)
	{
		addChildren(__tmpl, stack, m);
	}
	m->addWidget(stack);
}

void addChildren(MoldableTemplate* __tmpl,
		Wt::WContainerWidget* c, Moldable* m)
{
	std::string chnode = __tmpl->node+".children";
	ListNodeGenerator gen(chnode,__tmpl->num_children);
	for (int i = 0; i < __tmpl->num_children; i++)
	{
		MoldableTemplate* t = conceptualize(gen.next());
		Moldable* w = sculpt(t);
		c->addWidget( (Wt::WContainerWidget*) w);
		if (m == 0) m = (Moldable*) c;
		m->addMoldableChild(w);
	}
}

void __sculpt_text(MoldableTemplate* __tmpl, Moldable *m)
{
	mApp;
	if (__tmpl->style != EMPTY) setStyle(__tmpl->style,m);
	if ( (__tmpl->flags & is_dynamic) &&
			app->sessionID() != "global")

	{
		std::string content =
				Sessions::SubmissionHandler::dynamicLookup(
						remove_widget_prefix(__tmpl->node));
		Nodes::NodeValue v;
		Parsers::NodeValueParser p(content,&v,m);
		__tmpl->content = v.getString();
	}
	Wt::WString txt(__tmpl->content);

	if (m->children().size() > 0)
	{
		Wt::WText* t = (Wt::WText*) m->widget(0);
		t->setText(txt);
	}
	else
	{
		Wt::WContainerWidget* wtcw = (Wt::WContainerWidget*) m;
		wtcw->addWidget(new Wt::WText(txt));
	}
	m->setValueCallback(&Callbacks::__value_wtext);
	m->setSetValueCallback(&Callbacks::__setvalue_wtext);
}


}}}
