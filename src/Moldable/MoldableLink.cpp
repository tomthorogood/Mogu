/*
 * MoldableLink.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#include "MoldableLink.h"
#include <Redis/ContextQuery.h>
#include <Mogu.h>
#include <Redis/NodeEditor.h>

MoldableLink::MoldableLink (const std::string& node)
: Moldable(node, MoguSyntax::anchor)
{
    __init__();
}

void MoldableLink::__init__()
{
    mApp;
    NodeValue v;
    Redis::NodeEditor node(Prefix::widgets, __node);
    initializeNodeEditor(node);
    app->interpreter().giveInput(getParameter(node,MoguSyntax::location),v);
    std::string href = stripquotes(v.getString());
    app->interpreter().giveInput(getParameter(node,MoguSyntax::text),v);
    std::string text = stripquotes(v.getString());

    __link = new Wt::WAnchor(href, text);
    //TODO Add support for toggling target
    __link->setTarget(Wt::TargetNewWindow);
    addWidget(__link);
}
