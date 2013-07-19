/*
 * MoldableContainer.h
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#ifndef MOLDABLECONTAINER_H_
#define MOLDABLECONTAINER_H_

#include "MoldableAbstractParent.h"

class MoldableContainer : public MoldableAbstractParent
{
public:
    MoldableContainer(WidgetAssembly* assembly)
    : MoldableAbstractParent(assembly, MoguSyntax::container){}

    //Containers have no textual value:
    inline virtual std::string moldableValue(){return EMPTY;}
    inline virtual void setMoldableValue(const std::string&){}

    // public access for adding children to the widget
    virtual void appendChild(Moldable* child) { addChild(child);}

};


#endif /* MOLDABLECONTAINER_H_ */
