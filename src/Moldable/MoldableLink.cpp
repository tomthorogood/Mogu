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
    std::string href = getParameter("location");
    std::string text = getParameter("contents");
    __link = new Wt::WAnchor(href, text);
    //TODO Add support for toggling target
    __link->setTarget(Wt::TargetNewWindow);
    addWidget(__link);
}
