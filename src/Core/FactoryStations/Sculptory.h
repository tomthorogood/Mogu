/*
 * Sculptory.h
 *
 *  Created on: Sep 28, 2012
 *      Author: tom
 */

#ifndef SCULPTORY_H_
#define SCULPTORY_H_

#include <declarations.h>
#include <Core/MoldableTemplate.h>

namespace Goo{
namespace MoldableFactory{
namespace Sculptory{

using namespace Parsers::StyleParser;
using namespace Application;
using namespace Enums::WidgetTypes;
using namespace Enums::SignalTypes;

inline void setStyle(const std::string& _style, Wt::WWidget* m)
{
	Wt::WString style(_style);
	m->setStyleClass(style);
}

inline void getNumChildren(MoldableTemplate* __tmpl)
{
	std::string node;
	if (__tmpl->type == foreach)
	{
		node = __tmpl->content;
		/*Currently, a foreach loop's content field
		 * will have '@data.source.blah.@'
		 */
		node = node.substr(1,node.length()-2);
	}
	else
	{
		node = __tmpl->node+".children";
	}
	Redis::command("llen",node);
	__tmpl->num_children = Redis::getInt();
}

inline void addChildren(MoldableTemplate* __tmpl,
		Wt::WContainerWidget* c, Moldable* m=0)
{
	std::string chnode = __tmpl->node+".children";
	ListNodeGenerator gen(chnode,__tmpl->num_children);
	for (int i = 0; i < __tmpl->num_children; i++)
	{
		MoldableTemplate* t = conceptualize(gen.next());
		Moldable* w = sculpt(t);
		c->addWidget(w);
		if (m == 0) m = (Moldable*) c;
		m->addMoldableChild(w);
	}
}

inline void __sculpt_stack(MoldableTemplate* __tmpl, Moldable* m)
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

inline void __sculpt_container(MoldableTemplate* __tmpl, Moldable *m)
{
	if (__tmpl->style != EMPTY) setStyle(__tmpl->style,m);
	if (__tmpl->num_children > 0)
	{
		addChildren(__tmpl,m);
	}
}
inline void __sculpt_text(MoldableTemplate* __tmpl, Moldable *m)
{
	if (__tmpl->style != EMPTY) setStyle(__tmpl->style,m);
	Wt::WString txt(__tmpl->content);
	Wt::WText* wtxt = new Wt::WText(txt);
	m->addWidget(wtxt);
}

inline void __sculpt_foreach(MoldableTemplate* __tmpl,Moldable*m)
{
	Wt::WContainerWidget* bearer =m;
	if (__tmpl->flags & is_stacked)
	{
		__sculpt_stack(__tmpl,m);
		bearer = (Wt::WContainerWidget*) m->widget(0);
	}
	getNumChildren(__tmpl);
	MoldableTemplate* cpy = new MoldableTemplate(*__tmpl);
	Redis::command("hget", cpy->node, "template");
	std::string tpl_name = Redis::toString();
	cpy->type = getWidgetType(tpl_name);
	for (int i = 0; i < __tmpl->num_children; i++)
	{
		bearer->addWidget(sculpt(cpy));
	}
}
inline void __sculpt_link(MoldableTemplate* __tmpl,Moldable* m)
{
	if (__tmpl->style != EMPTY) setStyle(__tmpl->style,m);
	Wt::WAnchor* anchor = new Wt::WAnchor(
			__tmpl->location, __tmpl->content);
	anchor->setTarget(Wt::TargetNewWindow);
	m->addWidget(anchor);
}
inline void __sculpt_image(MoldableTemplate* __tmpl, Moldable* m)
{
	if (__tmpl->style != EMPTY) setStyle(__tmpl->style,m);
	Wt::WImage* img = new Wt::WImage(__tmpl->source, __tmpl->content);
	m->addWidget(img);
}

inline void __sculpt_image_link(MoldableTemplate* __tmpl, Moldable* m)
{
	if (__tmpl->style != EMPTY) setStyle(__tmpl->style,m);
	Wt::WImage* img = new Wt::WImage(__tmpl->source, __tmpl->content);
	Wt::WAnchor* a = new Wt::WAnchor(__tmpl->location, __tmpl->content);
	a->setImage(img);
	a->setTarget(Wt::TargetNewWindow);
	m->addWidget(a);
}

inline void __sculpt_button(MoldableTemplate* __tmpl,Moldable* m){}
inline void __sculpt_input_txt(MoldableTemplate* __tmpl, Moldable* m)
{
	if (__tmpl->style != EMPTY) setStyle(__tmpl->style,m);
	Wt::WLineEdit* in = new Wt::WLineEdit(__tmpl->content);
	if (__tmpl->flags & is_validated)
	{
		std::string val_name = getWidgetProperty(__tmpl->node, "validator");
		Wt::WValidator*  v= Validators::createValidator(val_name);
		in->setValidator(v);
		in->keyWentUp().connect(
				m, &Moldable::__validate);
	}
	m->addWidget(in);

}
inline void __sculpt_password_field(MoldableTemplate* __tmpl,Moldable* m)
{
	__sculpt_input_txt(__tmpl,m);
	Wt::WLineEdit* p = (Wt::WLineEdit*) m->widget(0);
	p->setEchoMode(Wt::WLineEdit::Password);
}

inline void __sculpt_textarea(MoldableTemplate* __tmpl,Moldable* m){}
inline void __sculpt_dropdown(MoldableTemplate* __tmpl,Moldable* m){}
inline void __sculpt_radio(MoldableTemplate* __tmpl,Moldable* m){}
inline void __sculpt_checkbox(MoldableTemplate* __tmpl,Moldable* m){}
inline void __sculpt_multi_select(MoldableTemplate* __tmpl,Moldable* m){}

}//namespace Sculptory
}//namespace MoldableFactory
}//namespace Goo

#endif /* SCULPTORY_H_ */
