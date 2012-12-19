/*
 * MoldableStack.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */


#include "MoldableStack.h"
#include <Wt/WAnimation>

MoldableStack::MoldableStack(const std::string& node)
: MoldableAbstractParent(node)
{
    __init__();
}

void MoldableStack::__init__()
{
    //MoldableAbstractParent::__init__();
    mApp;
    __stack = new Wt::WStackedWidget();
    std::string param = getParameter("animation");
    if (param != EMPTY) {
        NodeValue v;
        app->interpreter().giveInput(param, v);
        Wt::WAnimation transition( (Wt::WAnimation::AnimationEffect) v.getInt());
        __stack->setTransitionAnimation(transition, true);
    }
    Wt::WContainerWidget::addWidget(__stack);
}
