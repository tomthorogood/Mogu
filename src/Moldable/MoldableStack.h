/*
 * MoldableStack.h
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#ifndef MOLDABLESTACK_H_
#define MOLDABLESTACK_H_

#include "Moldable.h"
#include <Mogu.h>

class MoldableStack : public Moldable
{
    Wt::WStackedWidget* __stack;
public:
    MoldableStack (const std::string& node);

    //Stacked containers do not have textual values
    inline virtual std::string moldableValue() { return EMPTY; }
    inline virtual void setMoldableValue(const std::string& str) {}

    inline virtual void addWidget(Wt::WWidget* moldable)
    {
        mApp;
        __stack->addWidget(moldable);
        }
    }
};


#endif /* MOLDABLESTACK_H_ */
