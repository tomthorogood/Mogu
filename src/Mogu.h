/*
 * preview.h
 *
 *  Created on: May 29, 2012
 *      Author: tom
 */

#ifndef PREVIEW_H_
#define PREVIEW_H_

#include <declarations.h>
#include <Wt/WApplication>

struct WidgetRegistration
{
	Goo::Moldable* pointer;
	bool trunk;
	TurnLeft::Utils::HungryVector <std::string> children;
	WidgetRegistration();
};


class Mogu : public Wt::WApplication
{
	void handlePathChange(std::string path);
	WidgetRegister widgetRegister;
	Goo::Moldable* __wrapper;

	void resolveRegisteredPaths();

public:
    Mogu(const Wt::WEnvironment& env);
	bool widgetIsRegistered(std::string name);
	void registerWidget(std::string name, Goo::Moldable* widget);
};


#endif /* PREVIEW_H_ */
