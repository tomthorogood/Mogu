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
    __init__();
}

void MoldableImageLink::__init__()
{
    //MoldableLink::__init__();
    Redis::ContextQuery db(Prefix::widgets);
    mApp;
    NodeValue v;
    app->interpreter().giveInput(
        getParameter(db,MoguSyntax::source),v);
    std::string src = v.getString();
    __image = new Wt::WImage(src, moldableValue());
    __link->setImage(__image);
    __link->setTarget(Wt::TargetNewWindow);
}
