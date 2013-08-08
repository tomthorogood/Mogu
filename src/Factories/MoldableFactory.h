/*
 * Moldable_Factory.h
 *
 *  Created on: Sep 27, 2012
 *      Author: tom
 */

#ifndef MOLDABLEFACTORY_H_
#define MOLDABLEFACTORY_H_

#include <declarations.h>

class Moldable_Factory
{
public:
    Moldable_Factory() {}

    Moldable* create_moldable_widget (const std::string& node) const;

private:
    Widget_Server server {};
};


#endif /* MOLDABLEFACTORY_H_ */
