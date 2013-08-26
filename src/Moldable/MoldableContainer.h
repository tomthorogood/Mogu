/*
 * Moldable_Container.h
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#ifndef MOLDABLECONTAINER_H_
#define MOLDABLECONTAINER_H_

#include "MoldableAbstractParent.h"

class Moldable_Container : public Moldable_Abstract_Parent
{
public:
    Moldable_Container(Widget_Assembly& assembly)
    : Moldable_Abstract_Parent(assembly, Mogu_Syntax::container){}

    //Containers have no textual value:
    inline virtual std::string get_moldable_value(){return "";}
    inline virtual void set_moldable_value(const std::string&) {}

    // public access for adding children to the widget
    virtual void append_child(Moldable* child) { addChild(child);}

};


#endif /* MOLDABLECONTAINER_H_ */
