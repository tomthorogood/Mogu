/*
 * Sculptory.cpp
 *
 *  Created on: Aug 9, 2012
 *      Author: tom
 */

#include <declarations.h>
#include <Parsers/StyleParser.h>
#include <Core/Moldable.h>
#include <Static.h>
#include <Wt/WString>
#include <Wt/WAnchor>
#include <Wt/WImage>
#include <Wt/WText>
#include <Wt/WStackedWidget>
#include <Wt/WLineEdit>
#include <Mogu.h>
#include <Core/Sculptory.h>
#include <Validators/Validators.h>

namespace Goo{

using namespace Parsers::StyleParser;
using namespace Application;
using namespace Enums::WidgetTypes;
using namespace Enums::SignalTypes;
using namespace Enums::BitMasks;
using std::string;


void conceptualize (Moldable* widget)
{
	GooVariables* vars = widget->getProperties();
	std::string widgetNode = widget->getNode();
	WidgetTypes type = getWidgetType(widgetNode);
	vars->type = type;

	if (widgetIsDynamic(widgetNode))
	{
		vars->flags |= is_dynamic;
	}

	if (widgetHasProperty(widgetNode, "animation"))
	{
		vars->flags |= has_animation;
	}

	if (widgetHasProperty(widgetNode, "events"))
	{
		vars->flags |= has_events;
	}

	if (widgetBlocksActions(widgetNode))
	{
		vars->flags |= blocks_actions;
	}

	if (widgetHasChildren(widgetNode))
	{
		vars->flags |= has_children;
	}

	if (widgetHasProperty(widgetNode, "validator"))
	{
		vars->flags |= is_validated;
	}

	if (widgetHasProperty(widgetNode, "name"))
	{
		vars->flags |= is_named;
		std::string name = getWidgetProperty(widgetNode, "name");
		Application::mogu()->registerWidget(name, widget);
	}

	if ( (type & stack) == stack)
	{
		vars->flags |= is_stacked;
	}

	if (widgetHasProperty(widgetNode, "class"))
	{
		Wt::WString new_style;
		new_style = getWidgetProperty(widgetNode, "class");
		widget->setStyleClass(new_style);
	}

	// Mask HO bits to get true widget type
	if ( (type & WIDGET_HO_BITS) > stack)
	{
		vars->content = getWidgetText(widgetNode);
	}

	if ((type & WIDGET_HO_BITS) == image)
	{
		if (( (type & image) == image)
			|| ( (type & image_link) == image_link))
		{
			vars->source = getWidgetProperty(widgetNode, "source");
		}
	}

	namespace Type = Enums::WidgetTypes;
	if (type <= Type::link)
	{
		if ( ( (type & Type::link) == Type::link)
				|| ( (type & image_link) == image_link))
		{
			vars->location = getWidgetProperty(widgetNode, "location");
		}
	}
}


void mold(Moldable* widget)
{
	GooVariables* vars = widget->getProperties();
	std::string node = widget->getNode();


	if (vars->flags & blocks_actions)
	{
		vars->actionBlocking = getActionBlock(node);
	}

	if (widgetHasProperty(node, "tooltip"))
	{
		Wt::WString tooltip = getWidgetProperty(node, "tooltip");
		Wt::TextFormat format = Wt::XHTMLText;
		widget->setToolTip(tooltip,format);
	}

	if ( (vars->type & WIDGET_HO_BITS) == widget_usually_clicked)
	{
		Wt::WAnchor* anchor = new Wt::WAnchor(
				vars->location,
				vars->content);
		if ( (vars->type & WIDGET_HO_BITS) == image_link)
		{
			Wt::WImage* image = new Wt::WImage(
					vars->source, vars->content);
			anchor->setImage(image);
		}
		widget->addWidget(anchor);
	}

	else if ( (vars->type & image) == image)
	{
		Wt::WImage* image = new Wt::WImage(
				vars->source, vars->content);
		widget->addWidget(image);
	}

	else
	{
		switch(vars->type & WIDGET_HO_BITS)
		{

		case text:{
			Wt::WText* text = new Wt::WText(vars->content);
			widget->addWidget(text);
			break;}

		case stack:{
			Wt::WStackedWidget* stack = new Wt::WStackedWidget();
			if (vars->flags & has_animation)
			{
				Wt::WAnimation::AnimationEffect effect =
						getWidgetAnimation(node);
				Wt::WAnimation transition(effect);
				stack->setTransitionAnimation(transition,true);
			}
			if (widgetHasProperty(node, "class"))
			{
				stack->setStyleClass(widget->styleClass());
				widget->setStyleClass("");
			}
			widget->addWidget(stack);
			break;}

		case input_text:
		case password:{
			Wt::WLineEdit* input = new Wt::WLineEdit(vars->content);
			if ((vars->type & WIDGET_HO_BITS) == password)
			{
				input->setEchoMode(Wt::WLineEdit::Password);
			}
			widget->addWidget(input);
			if (vars->flags & is_validated)
			{
				std::string val_name = getWidgetProperty(node, "validator");
				Wt::WValidator* validator =
						Validators::createValidator(val_name);
				input->setValidator(validator);
				input->keyWentUp().connect(
						widget, &Moldable::__validate);
			}
			}
			break;
		}
	}

	if (vars->flags & has_children)
	{
		getWidgetChildren(node, vars->children);
		int num_children = vars->children.size();
		for (int i = 0; i < num_children; i++)
		{
			std::string name = vars->children.at(i);
			widget->addGoo(name);
		}
	}

}

}//namespace Goo

