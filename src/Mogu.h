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

public:
    Mogu(const Wt::WEnvironment& env);
	bool searchPathTree(std::string name) const;
	void registerWithParent(
			std::string parent, int child_index, std::string child_name);
};


#endif /* PREVIEW_H_ */
