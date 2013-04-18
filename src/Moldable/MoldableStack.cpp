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

MoldableStack::MoldableStack(const std::string& node)
: MoldableAbstractParent(node)
{
    __init__();
}

void MoldableStack::__init__()
{

}
