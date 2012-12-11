/*
 * MoldableImageLink.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#include "MoldableImageLink.h"
#include <Mogu.h>
#include <Wt/WAnchor>
#include <Wt/WImage>

MoldableImageLink::MoldableImageLink(const std::string& node)
: MoldableLink(node)
{
    std::string param = getParameter("source");
    mApp;
    NodeValue v;
    app->interpreter().giveInput(param,v);
    std::string src = v.getString();
    __image = new Wt::WImage(src, moldableValue());
    __link->setImage(__image);
    __link->setTarget(Wt::TargetNewWindow);
}


