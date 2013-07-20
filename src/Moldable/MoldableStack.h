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
    WFriendlyStack* stack = nullptr;
    Wt::Signal <> stack_index_changed;

public:
    MoldableStack (WidgetAssembly*);

    //Stacked containers do not have textual values.
    inline virtual std::string moldableValue() { return node; }
    inline virtual void setMoldableValue(const std::string& str) {}

    virtual void appendChild(Moldable* child) {
        stack->addChild(child);
    }

    inline virtual void addWidget(Wt::WWidget* moldable)
    {
        stack->addWidget(moldable);
    }

    inline Wt::Signal <>& stackIndexChanged() { return stack_index_changed;}

    inline virtual void init()
    {
        // Don't cause memory leaks when we reload the widget.
        if (stack != nullptr) {
            removeWidget(stack);
            delete stack;
        }

        stack = new WFriendlyStack();
        MoldableAbstractParent::addWidget(stack);
    }

    inline virtual void addChild(Moldable* child)  {
        stack->addChild(child);
    }

    inline virtual void removeChild(Moldable* child)  {
        stack->removeChild(child);
    }
};


#endif /* MOLDABLESTACK_H_ */
