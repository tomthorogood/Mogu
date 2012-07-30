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
	RegisteredPaths registered_paths;
	Goo::Moldable* __wrapper;

public:
    Mogu(const Wt::WEnvironment& env);
	bool searchPathTree(std::string name);
	void registerPath(std::string name, WidgetRegistration* record);
	void registerWithParent(std::string parent_name, std::string child_name);
};


#endif /* PREVIEW_H_ */
