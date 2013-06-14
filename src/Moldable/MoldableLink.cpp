/*
 * MoldableLink.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#include "MoldableLink.h"
#include <Redis/ContextQuery.h>
#include <Mogu.h>

MoldableLink::MoldableLink (const std::string& node)
: Moldable(node, MoguSyntax::anchor)
{
    __init__();
}

void MoldableLink::__init__()
{
    mApp;
    NodeValue v;
    Redis::ContextQuery db(Prefix::widgets);

    app->interpreter().giveInput(getParameter(db,MoguSyntax::location),v);
    std::string href = v.getString();
    app->interpreter().giveInput(getParameter(db,MoguSyntax::text),v);
    std::string text = v.getString();

    __link = new Wt::WAnchor(href, text);
    //TODO Add support for toggling target
    __link->setTarget(Wt::TargetNewWindow);
    addWidget(__link);
}
