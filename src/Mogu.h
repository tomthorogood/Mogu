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

class Mogu : public Wt::WApplication
{
	void handlePathChange(std::string path);
	StackedWidgetTree __path_tree;
	bool searchPathTree(std::string name);
	void registerWithParent(
			std::string parent, int child_index, std::string child_name);
public:
    Mogu(const Wt::WEnvironment& env);
};


#endif /* PREVIEW_H_ */
