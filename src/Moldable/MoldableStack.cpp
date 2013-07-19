/*
 * Mold    MoldableAbstractParent::__init__();
    __stack = new Wt::WStackedWidget();
    Wt::WContainerWidget::addWidget(__stack);ableStack.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */


#include "MoldableStack.h"
#include <Redis/ContextQuery.h>
#include <Wt/WAnimation>
#include <Types/WidgetAssembly.h>

MoldableStack::MoldableStack(WidgetAssembly* assembly)
: MoldableAbstractParent(assembly, MoguSyntax::stack)
{
    __init__();
}
