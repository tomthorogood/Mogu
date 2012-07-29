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

Mogu::Mogu(const Wt::WEnvironment& env)
:   Wt::WApplication(env)
{
	Application::defineMogu(this);
    std::string styleSheet("/resources/mogu/style.css");
    useStyleSheet(styleSheet);

    std::string outermost_container = "widgets.wrapper";

    /* This will be deleted by the wrapper! */
    Goo::Moldable* moldableWrapper =
            new Goo::Moldable(outermost_container);
    root()->addWidget(moldableWrapper);
    internalPathChanged().connect(this, &Mogu::handlePathChange);
}

bool Mogu::searchPathTree(std::string widget_name)
{
	StackedWidgetTree::iterator iter;
	iter = __path_tree.find(widget_name);
	return (iter != __path_tree.end());
}

void Mogu::registerWithParent(
		std::string parent_name,
		int child_index,
		std::string child_name)
{
	__path_tree[parent_name][child_name] = child_index;
}

void Mogu::handlePathChange(std::string path)
{
//TODO this.
}
