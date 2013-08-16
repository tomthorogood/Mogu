/*
 * Moldable_Stack.h
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

class Moldable_Stack : public Moldable_Abstract_Parent
{
private:
    WFriendlyStack* stack {};
    Wt::Signal <> stack_index_changed;

public:
    Moldable_Stack (Widget_Assembly*);

    //Stacked containers do not have textual values.
    inline virtual std::string get_moldable_value() {return "";}

    inline virtual void set_moldable_value(const std::string& str) {}

    virtual void append_child(Moldable* child)
        { stack->addChild(child); }

    inline virtual void addWidget(Wt::WWidget* moldable)
        { stack->addWidget(moldable); }

    inline Wt::Signal <>& stackIndexChanged()
        { return stack_index_changed;}

    inline virtual void init()
    {
        // Don't cause memory leaks when we reload the widget.
        if (stack)
        {
            removeWidget(stack);
            delete stack;
            stack=nullptr;
        }

        stack = new WFriendlyStack();
        Moldable_Abstract_Parent::addWidget(stack);
    }

    inline virtual void addChild(Moldable* child)  {
        stack->addChild(child);
    }

    inline virtual void removeChild(Moldable* child)  {
        stack->removeChild(child);
    }
};


#endif /* MOLDABLESTACK_H_ */
