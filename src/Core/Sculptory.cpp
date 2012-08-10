/*
 * Sculptory.cpp
 *
 *  Created on: Aug 9, 2012
 *      Author: tom
 */

#include <declarations.h>
#include <Helpers.h>
#include <Parsers/StyleParser.h>
#include <Static.h>
#include <Wt/WString>
#include <Wt/WAnchor>
#include <Wt/WImage>
#include <Wt/WText>
#include <Wt/WStackedWidget>
#include <Wt/WLineEdit>
#include <Mogu.h>
#include <Core/Sculptory.h>

namespace Goo{

using namespace Parsers::StyleParser;
using namespace Enums::WidgetTypes;
using namespace Enums::SignalTypes;
using namespace Enums::BitMasks;

void conceptualize (Moldable* widget)
{
	GooVariables* vars = widget->getProperties();
	WidgetTypes type = getWidgetType(widget);
	vars->type = type;

	if (widgetHasAnimation(widget))
	{
		vars->flags |= has_animation;
	}

	if (widgetHasEvents(widget))
	{
		vars->flags |= has_events;
	}

	if (widgetBlocksActions(widget))
	{
		vars->flags |= blocks_actions;
	}

	if (widgetHasChildren(widget))
	{
		vars->flags |= has_children;
	}

	if (widgetIsNamed(widget))
	{
		vars->flags |= is_named;
		std::string name = getWidgetName(widget);
		Application::mogu()->registerWidget(name, widget);
	}

	if ( (type & stack) == stack)
	{
		vars->flags |= is_stacked;
	}

	if (widgetHasStyling(widget))
	{
		Wt::WString new_style(getWidgetStyleClass(widget));
		widget->setStyleClass(new_style);
	}

	// Mask HO bits to get true widget type
	if ( (type & WIDGET_HO_BITS) > stack)
	{
		vars->content = getWidgetText(widget);
	}

	if (( (type & image) == image)
		|| ( (type & image_link) == image_link))
	{
		vars->source = getWidgetImgSource(widget);
	}

	namespace Type = Enums::WidgetTypes;
	if ( ( (type & Type::link) == Type::link)
			|| ( (type & image_link) == image_link))
	{
		vars->location = getWidgetLinkLocation(widget);
	}
}

void mold(Moldable* widget)
{
	GooVariables* vars = widget->getProperties();

	if (vars->flags & blocks_actions)
	{
		vars->actionBlocking = getActionBlock(widget);
	}

	if ( (vars->type & widget_usually_clicked) == widget_usually_clicked)
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
						getWidgetAnimation(widget);
				Wt::WAnimation transition(effect);
				stack->setTransitionAnimation(transition,true);
			}
			stack->setStyleClass(widget->styleClass());
			widget->setStyleClass("");
			widget->addWidget(stack);
			break;}

		case input_text:{
			Wt::WLineEdit* input = new Wt::WLineEdit(vars->content);
			widget->addWidget(input);
			break;}
		}
	}

	if (vars->flags & has_children)
	{
		getWidgetChildren(widget, vars->children);
		int num_children = vars->children.size();
		for (int i = 0; i < num_children; i++)
		{
			std::string name = vars->children.at(i);
			widget->addGoo(name);
		}
	}

}

}//namespace Goo

