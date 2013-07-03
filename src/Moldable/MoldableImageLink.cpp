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
#include <Redis/NodeEditor.h>

MoldableImageLink::MoldableImageLink(const std::string& node)
: MoldableLink(node)
{
    __init__();
}

void MoldableImageLink::__init__()
{
    Redis::NodeEditor node(Prefix::widgets, __node);
    initializeNodeEditor(node);
    mApp;
    NodeValue v;
    app->interpreter().giveInput(
        getParameter(node,MoguSyntax::source),v);
    std::string src = stripquotes(v.getString());
    __image = new Wt::WImage(src, moldableValue());
    __link->setImage(__image);
    __link->setTarget(Wt::TargetNewWindow);
}
