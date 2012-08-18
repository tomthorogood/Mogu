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
#include <signal.h>
#include <Redis/RedisCore.h>

#ifndef AUTH_TOKEN
#define AUTH_TOKEN "BendTheWeb"
#endif

#ifndef GLOBAL_SESSION
#define GLOBAL_SESSION "global"
#endif

class Mogu : public Wt::WApplication
{
	void handlePathChange(std::string path);
	WidgetRegister widgetRegister;
	Goo::Moldable* __wrapper;

public:

	Mogu(const Wt::WEnvironment& env);
	virtual ~Mogu();
	bool widgetIsRegistered(std::string name);
	void registerWidget(std::string name, Goo::Moldable* widget);
	Goo::Moldable* registeredWidget(std::string name);
	void deregisterWidget(std::string name);
};


#endif /* PREVIEW_H_ */
