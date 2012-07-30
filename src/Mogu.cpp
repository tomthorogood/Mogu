/*
 * preview.cpp
 *
 *  Created on: Jul 17, 2012
 *      Author: tom
 */

#include <Mogu.h>
#include <string>
#include <Core/Moldable.h>
#include <Wt/WApplication>
#include <Wt/WScrollArea>
#include <Wt/WString>
#include <Static.h>
#include <algorithm>
#include <TurnLeftLib/Utils/explosion.h>
#include <Wt/WStackedWidget>

WidgetRegistration::WidgetRegistration()
:	children()
{
	pointer	=0;
	trunk	=false;
}


Mogu::Mogu(const Wt::WEnvironment& env)
:   Wt::WApplication(env)
{
	Application::defineMogu(this);
    std::string styleSheet("/resources/mogu/style.css");
    useStyleSheet(styleSheet);

    std::string outermost_container = "widgets.wrapper";

    /* This will be deleted by the wrapper! */
    __wrapper = new Goo::Moldable(outermost_container);
    root()->addWidget(__wrapper);
    internalPathChanged().connect(this, &Mogu::handlePathChange);
}

bool Mogu::searchPathTree(std::string widget_name)
{
	RegisteredPaths::iterator iter;
	iter = registered_paths.find(widget_name);

	return (iter != registered_paths.end());
}

void Mogu::registerPath(std::string name, WidgetRegistration* record)
{
	registered_paths[name] = record;
}

void Mogu::handlePathChange(std::string path)
{
	TurnLeft::Utils::Explosion explosion(path);
	std::string sections[16];
	explosion.explode('/', sections);
	int num_sections = explosion.getNumWords();

	for (
			int i = 0;
			i < num_sections-1; //Don't recurse into the last path appendage
			i++)
	{
		std::string name = sections[i];
		std::string next = sections[i+i];
		bool registered = searchPathTree(name);
		if (!registered) return;
		WidgetRegistration* record = registered_paths[name];
		Goo::Moldable* me = record->pointer;
		if (!record->trunk)
		{
			TurnLeft::Utils::HungryVector<std::string>::iterator iter;
			iter = std::find(
					record->children.begin(), record->children.end(),next);
			if (iter != record->children.end())
			{
				if (searchPathTree(next))
				{
					Goo::Moldable* child = registered_paths[next]->pointer;
					Wt::WStackedWidget* stack =
							(Wt::WStackedWidget*) me->widget(0);
					stack->setCurrentWidget(child);
				}
			}
		}
		else
		{
			if (me->isHidden())
			{
				me->show();
			}
		}
	}
}

void Mogu::registerWithParent(
		std::string parent_name, std::string child_name)
{
	registered_paths[parent_name]->children.add(child_name);
}
