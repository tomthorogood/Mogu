/*
 * preview.cpp
 *
 *  Created on: Jul 17, 2012
 *      Author: tom
 */


#include <Static.h>
#include <algorithm>


#include <Wt/WApplication>
#include <Wt/WScrollArea>
#include <Wt/WString>
#include <Wt/WStackedWidget>
#include <TurnLeftLib/Utils/explosion.h>

#include <Mogu.h>
#include <Core/Moldable.h>
#include <Parsers/StyleParser.h>
#include <Perspectives/PerspectiveHandler.h>
#include <hash.h>

#ifdef TERM_ENABLED
#include <Wt/WText>
#endif


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
	Application::setWtSession(sessionId());
	Application::ignoreBroadcaster(0);
	Application::setLastTrigger(Enums::SignalTriggers::fail);


	std::string global_auth = AUTH_TOKEN;
	std::string auth_hash = Hash::toHash(global_auth);
	Application::setAuthToken(auth_hash);
	Application::setSessionID(GLOBAL_SESSION);

    std::string styleSheet("/resources/mogu/style.css");
    useStyleSheet(styleSheet);

    std::string outermost_container = "widgets.wrapper";

    __wrapper = new Goo::Moldable(outermost_container);

    root()->addWidget(__wrapper);

    internalPathChanged().connect(this, &Mogu::handlePathChange);

    std::string entry_path = internalPath();
    if (entry_path != "/" && entry_path.length() > 0)
    {
    	handlePathChange(entry_path);
    }
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

void Mogu::deregisterWidget(std::string name)
{
	if (widgetIsRegistered(name))
	{
		WidgetRegister::iterator iter = widgetRegister.find(name);
		widgetRegister.erase(iter);
	}
}

Goo::Moldable* Mogu::registeredWidget(std::string name)
{
	return widgetRegister[name];
}

void Mogu::handlePathChange(std::string path)
{
	std::string perspective = path.substr(1,path.length()-1);
	TurnLeft::Utils::Explosion explosion(perspective);
	std::string molds[16];
	explosion.explode('/',molds);
	int num_molds = explosion.getNumWords();
	for (int m = 0; m < num_molds; m++)
	{
		std::string mold = molds[m];
		Perspective::Handler::mold(mold);
	}
}

Mogu::~Mogu()
{
}
