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
    MoldableFactory(Mogu* app);
    ~MoldableFactory();

    Moldable* createMoldableWidget(const std::string& node) const;

private:
    WidgetServer* server    =nullptr;
};


#endif /* MOLDABLEFACTORY_H_ */
