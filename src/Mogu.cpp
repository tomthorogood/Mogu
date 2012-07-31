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
#include <Parsers/StyleParser.h>
#include <Perspectives/PerspectiveHandler.h>

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

bool Mogu::widgetIsRegistered(std::string widget_name)
{
	WidgetRegister::iterator iter;
	iter = widgetRegister.find(widget_name);

	return (iter != widgetRegister.end());
}

void Mogu::registerWidget(std::string name, Goo::Moldable* widget)
{
	widgetRegister[name] = widget;
}

void Mogu::handlePathChange(std::string path)
{
	std::string perspective = path.substr(1,path.length()-1);
	Perspective::Handler::mold(perspective);
}
