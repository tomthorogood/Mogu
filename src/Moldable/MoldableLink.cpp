/*
 * MoldableLink.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#include "MoldableLink.h"
#include <Mogu.h>

MoldableLink::MoldableLink (const std::string& node)
: Moldable(node)
{
    __init__();
}

void MoldableLink::__init__()
{
    ///Moldable::__init__();
    mApp;
    NodeValue v;
    std::string param = getParameter("location");
    app->interpreter().giveInput(param,v);
    std::string href = v.getString();
    param = getParameter("content");
    app->interpreter().giveInput(param,v);
    std::string text = v.getString();
    __link = new Wt::WAnchor(href, text);
    //TODO Add support for toggling target
    __link->setTarget(Wt::TargetNewWindow);
    addWidget(__link);
}
