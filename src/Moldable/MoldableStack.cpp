/*
 * MoldableStack.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */


#include "MoldableStack.h"

MoldableStack::MoldableStack(const std::string& node)
: Moldable(node)
{
    mApp;
    __stack = new Wt::WStackedWidget();
    std::string param = getParameter("animation");
    if (param != EMPTY) {
        NodeValue v;
        app->interpreter().giveInput(param, v, NULL
            , Parsers::enum_callback <Parsers::WtAnimationParser>);
        Wt::WAnimation transition( (Wt::WAnimation::AnimationEffect) v.getInt);
        __stack->animateHide(transition);
        __stack->animateShow(transition);
    }
    Wt::WContainerWidget::addWidget(__stack);
}

