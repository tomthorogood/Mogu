/*
 * MoldableFactory.cpp
 *
 *  Created on: Sep 27, 2012
 *      Author: tom
 */

#include <Core/MoldableFactory.h>
#include <Core/FactoryStations/Sculptory.h>
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

#include <Mogu.h>

#include <Parsers/StyleParser.h>
#include <Validators/Validators.h>
#include <Types/Enums.h>
#include <Core/Moldable.h>

namespace Goo{
namespace MoldableFactory{

using namespace Parsers::StyleParser;
using namespace Application;
using namespace Enums::WidgetTypes;
using namespace Sculptory;


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
#ifdef DEBUG
	assert(__tmpl->node != "");
#endif

	if (widgetHasProperty(node, "animation")) __tmpl->flags |= has_animation;
	if (widgetHasEvents(node)) __tmpl->flags |= has_events;
	if (widgetHasProperty(node, "block")) __tmpl->flags |= blocks_actions;
	if (widgetHasProperty(node, "validator")) __tmpl->flags |= is_validated;
	if (widgetHasProperty(node, "name")) __tmpl->flags |= is_named;
	if (widgetHasProperty(node, "tooltip")) __tmpl->flags |= has_tooltip;

	__tmpl->style = getWidgetProperty(node,"class");

	__tmpl->content = getWidgetProperty(node, "content");

	if (widgetHasProperty(node, "source"))
		__tmpl->source = getWidgetProperty(node,"source");

	if (widgetHasProperty(node, "location"))
		__tmpl->location = getWidgetProperty(node, "location");

	return __tmpl;
}

MoldableTemplate* conceptualize(const MoldableTemplate* __orig, size_t index)
{
	mApp;
	const std::string tpl_node = getWidgetProperty(__orig->node,"template");
	MoldableTemplate* t = conceptualize(tpl_node);
	std::string concept_loc = __orig->content;
	concept_loc += " " + itoa(index);
	Nodes::NodeValue v;
	app->interpreter().giveInput(concept_loc,v);
	t->content = v.getString();
	return t;
}

Moldable* sculpt(MoldableTemplate* __tmpl, Moldable* m)
{
	mApp;
	bool mnew = m==0;
	if (mnew) m = new Moldable(__tmpl);

	if (__tmpl->flags&is_named)
	{
		std::string name = (__tmpl->name != EMPTY) ?
					__tmpl->name
				:	 getWidgetProperty(__tmpl->node, "name");

		app->registerWidget(name, m);
	}
	if (__tmpl->flags&has_tooltip)
	{
		Wt::WString tooltip(getWidgetProperty(__tmpl->node, "tooltip"));
		m->setToolTip(tooltip, Wt::XHTMLText);
	}

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
	if (mnew)++__tmpl->num_connected_widgets;
	return m;
}

}//namespace MoldableFactory
}//namespace Goo


