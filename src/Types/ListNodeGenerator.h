/*
 * ListNodeGenerator.h
 *
 *  Created on: Sep 27, 2012
 *      Author: tom
 */

#ifndef LISTNODEGENERATOR_H_
#define LISTNODEGENERATOR_H_

#include <declarations.h>

class ListNodeGenerator
{
	size_t __max;
	size_t __current;
	std::string __node;
public:
	ListNodeGenerator(const std::string& node, size_t max=0);
	std::string next();
};


#endif /* LISTNODEGENERATOR_H_ */
