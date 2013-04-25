/*
 * MoldableStack.h
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#ifndef MOLDABLESTACK_H_
#define MOLDABLESTACK_H_

#include "MoldableAbstractParent.h"
#include <Mogu.h>
#include <Wt/WStackedWidget>

class MoldableStack : public MoldableAbstractParent
{
private:
    Wt::WStackedWidget* __stack;
    Wt::Signal <> __stack_index_changed;

public:
    MoldableStack (const std::string& node);

    //Stacked containers do not have textual values
    inline virtual std::string moldableValue() { return EMPTY; }
    inline virtual void setMoldableValue(const std::string& str) {}

    inline virtual void addWidget(Wt::WWidget* moldable)
    {
        __stack->addWidget(moldable);
    }

    inline Wt::Signal <>& stackIndexChanged() { return __stack_index_changed;}

    inline virtual void reload()
    {
        force_reload = true;
        __init__();
        load();
        force_reload = false;
    }

    inline virtual void __init__()
    {
        MoldableAbstractParent::__init__();
        __stack = new Wt::WStackedWidget();
    }
};


#endif /* MOLDABLESTACK_H_ */
