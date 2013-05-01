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

class WFriendlyStack : public Wt::WStackedWidget
{
public:
    inline virtual void addChild(Moldable* child) {
        Wt::WStackedWidget::addChild(
                static_cast<Wt::WWebWidget*>(child));
    }

    inline virtual void removeChild(Moldable* child) {
        Wt::WStackedWidget::removeChild(
                static_cast<Wt::WWebWidget*>(child));
    }
};

class MoldableStack : public MoldableAbstractParent
{
private:
    WFriendlyStack* __stack;
    Wt::Signal <> __stack_index_changed;

public:
    MoldableStack (const std::string& node);

    //Stacked containers do not have textual values
    inline virtual std::string moldableValue() { return EMPTY; }
    inline virtual void setMoldableValue(const std::string& str) {}

    virtual void appendChild(Moldable* child) {
        __stack->addChild(child);
    }

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
        __stack = new WFriendlyStack();
    }

    inline virtual void addChild(Moldable* child) override {
        __stack->addChild(child);
    }

    inline virtual void removeChild(Moldable* child) override {
        __stack->removeChild(child);
    }
};


#endif /* MOLDABLESTACK_H_ */
