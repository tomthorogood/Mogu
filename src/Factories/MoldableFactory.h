/*
 * MoldableFactory.h
 *
 *  Created on: Sep 27, 2012
 *      Author: tom
 */

#ifndef MOLDABLEFACTORY_H_
#define MOLDABLEFACTORY_H_

#include <declarations.h>

class MoldableFactory
{
public:
    MoldableFactory();
    Moldable* createMoldableWidget(const std::string& node) const;
};


#endif /* MOLDABLEFACTORY_H_ */
