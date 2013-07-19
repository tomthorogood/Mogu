/*
 * MoldableStack.h
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#ifndef MOLDABLESTACK_H_
#define MOLDABLESTACK_H_

#include "MoldableAbstractParent.h"
#include <Wt/WStackedWidget>

/*
 * Since Wt standard behaviour does not allow external widgets to add children
 * to others, we have to override this default behaviour for Mogu to work.
 */
class WFriendlyStack : public Wt::WStackedWidget
{
public:
    inline virtual void addChild(Moldable* child) {
        Wt::WStackedWidget::addChild(child);
    }

    inline virtual void removeChild(Moldable* child) {
        Wt::WStackedWidget::removeChild(child);
    }
};

class MoldableStack : public MoldableAbstractParent
{
private:
    WFriendlyStack* __stack = nullptr;
    Wt::Signal <> __stack_index_changed;

public:
    MoldableStack (WidgetAssembly*);

    //Stacked containers do not have textual values.
    inline virtual std::string moldableValue() { return __node; }
    inline virtual void setMoldableValue(const std::string& str) {}

    virtual void appendChild(Moldable* child) {
        __stack->addChild(child);
    }

    inline virtual void addWidget(Wt::WWidget* moldable)
    {
        __stack->addWidget(moldable);
    }

    inline Wt::Signal <>& stackIndexChanged() { return __stack_index_changed;}

    inline virtual void __init__()
    {
        // Don't cause memory leaks when we reload the widget.
        if (__stack != nullptr) {
            removeWidget(__stack);
            delete __stack;
        }

        __stack = new WFriendlyStack();
        MoldableAbstractParent::addWidget(__stack);
    }

    inline virtual void addChild(Moldable* child)  {
        __stack->addChild(child);
    }

    inline virtual void removeChild(Moldable* child)  {
        __stack->removeChild(child);
    }
};


#endif /* MOLDABLESTACK_H_ */
