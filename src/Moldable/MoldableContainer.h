/*
 * MoldableContainer.h
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#ifndef MOLDABLECONTAINER_H_
#define MOLDABLECONTAINER_H_

#include "Moldable.h"

class MoldableContainer : public Moldable
{
public:
    MoldableContainer(const std::string& node) : Moldable(node){}

    //Containers have no textual value:
    inline virtual std::string moldableValue(){return EMPTY;}
    inline virtual void setMoldableValue(const std::string&){}

};


#endif /* MOLDABLECONTAINER_H_ */
