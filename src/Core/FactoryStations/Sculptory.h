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
#include <Core/MoldableFactory.h>
#include <Redis/RedisCore.h>
#include <Wt/WString>
#include <Wt/WWidget>
#include <Static.h>
#include <Parsers/StyleParser.h>
#include <Types/ListNodeGenerator.h>
#include <Wt/WContainerWidget>
#include <Validators/Validators.h>
#include <Wt/WAnchor>
#include <Wt/WImage>
#include <Wt/WText>
#include <Core/Moldable.h>
#include <Sessions/Lookups.h>
#include <Sessions/Submission.h>

namespace Goo{
namespace MoldableFactory{
namespace Sculptory{

using namespace Parsers::StyleParser;
using namespace Application;
using namespace Enums::WidgetTypes;
using namespace Enums::SignalTypes;

void __sculpt_stack(MoldableTemplate* __tmpl, Moldable* m);
void addChildren(MoldableTemplate* __tmpl,
		Wt::WContainerWidget* c, Moldable* m=0);
void __sculpt_text(MoldableTemplate* __tmpl, Moldable *m);


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
	mApp;
	app->redisCommand("llen",node);
	__tmpl->num_children = Redis::getInt(app->reply());
}





inline void __sculpt_container(MoldableTemplate* __tmpl, Moldable *m)
{
	if (__tmpl->style != EMPTY) setStyle(__tmpl->style,m);
	if (__tmpl->num_children > 0)
	{
		addChildren(__tmpl,m);
	}
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
	for (int i = 0; i < __tmpl->num_children; i++)
	{
		MoldableTemplate* cpy = new MoldableTemplate(*__tmpl);
		cpy->num_children =0;
		std::string tpl_name = getWidgetProperty(__tmpl->node, "template");
		std::string datanode = cpy->content.substr(1,cpy->content.length()-2);
		mApp;
		app->redisCommand("lindex", datanode, itoa(i));
		cpy->content = Redis::toString(app->reply());
		cpy->type = getWidgetType("templates."+tpl_name);
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
	m->setValueCallback(&Callbacks::__value_wlineedit);

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
