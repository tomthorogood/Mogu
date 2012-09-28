/*
 * MoldableFactory.cpp
 *
 *  Created on: Sep 27, 2012
 *      Author: tom
 */

#include <Core/MoldableFactory.h>
#include <Types/ListNodeGenerator.h>
#include <Static.h>
#include <Wt/WWidget>
#include <Wt/WString>
#include <Wt/WLineEdit>
#include <Wt/WStackedWidget>
#include <Wt/WContainerWidget>
#include <Wt/WAnchor>
#include <Wt/WImage>
#include <Wt/WText>
#include <Wt/WAnimation>

#include <Parsers/StyleParser.h>
#include <Parsers/NodeValueParser.h>
#include <Validators/Validators.h>
#include <Types/Enums.h>
#include <Core/Moldable.h>

namespace Goo{
namespace MoldableFactory{

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


/*!\brief Using a redis database node, puts together the template for a
 * Moldable object, but does not instantiate it.
 * @param node The "widgets.x" node.
 * @return A prototype/template of the Moldable object.
 */
MoldableTemplate* conceptualize(const std::string& node)
{
	MoldableTemplate* __tmpl = new MoldableTemplate();
	__tmpl->type = getWidgetType(node);

	__tmpl->node = node;

	if (widgetIsDynamic(node)) __tmpl->flags |= is_dynamic;
	if (widgetHasProperty(node, "animation")) __tmpl->flags |= has_animation;
	if (widgetHasProperty(node,"events")) __tmpl->flags |= has_events;
	if (widgetHasProperty(node, "block")) __tmpl->flags |= blocks_actions;
	if (widgetHasProperty(node, "validator")) __tmpl->flags |= is_validated;
	if (widgetHasProperty(node, "name")) __tmpl->flags |= is_named;
	if (widgetHasProperty(node, "tooltip")) __tmpl->flags |= has_tooltip;

	if (widgetHasProperty(node,"class"))
		__tmpl->style = getWidgetProperty(node,"class");

	if (widgetHasProperty(node, "content"))
		__tmpl->content = getWidgetProperty(node, "content");

	if (widgetHasProperty(node, "source"))
		__tmpl->content = getWidgetProperty(node,"source");

	if (widgetHasProperty(node, "location"))
		__tmpl->location = getWidgetProperty(node, "location");

	return __tmpl;
}

MoldableTemplate* conceptualize(const MoldableTemplate* __orig, size_t index)
{
	const std::string tpl_node = getWidgetProperty(__orig->node,"template");
	MoldableTemplate* t = conceptualize(tpl_node);
	std::string concept_loc = __orig->content;
	concept_loc += " " + itoa(index);
	Nodes::NodeValue v;
	Parsers::NodeValueParser parser(concept_loc,&v);
	t->content = v.getString();
	return t;
}

Moldable* sculpt(MoldableTemplate* __tmpl)
{
	Moldable* m = new Moldable(__tmpl);
	switch(__tmpl->type)
	{
	case container:{
		getNumChildren(__tmpl);
		__sculpt_container(__tmpl,m);
		break;}
	case stack:{
		getNumChildren(__tmpl);
		__sculpt_stack(__tmpl,m);
		break;}
	case foreach:{
		__sculpt_foreach(__tmpl,m);
		break;}
	case text:{
		__sculpt_text(__tmpl,m);
		break;}
	case image:{
		__sculpt_image(__tmpl,m);
		break;}
	case Enums::WidgetTypes::link:{
		__sculpt_link(__tmpl,m);
		break;}
	case image_link:{
		__sculpt_image_link(__tmpl,m);
		break;}
	case button:{
		__sculpt_button(__tmpl,m);
		break;}
	case input_text:{
		__sculpt_input_txt(__tmpl,m);
		break;}
	case textarea:{
		__sculpt_textarea(__tmpl,m);
		break;}
	case dropdown_menu:{
		__sculpt_dropdown(__tmpl,m);
		break;}
	case radio_button_menu:{
		__sculpt_radio(__tmpl,m);
		break;}
	case checkbox_menu:{
		__sculpt_checkbox(__tmpl,m);
		break;}
	case multi_select:{
		__sculpt_multi_select(__tmpl,m);
		break;}
	case password:{
		__sculpt_password_field(__tmpl,m);
		break;}
	}
	++__tmpl->num_connected_widgets;
	return m;
}

}//namespace MoldableFactory
}//namespace Goo


