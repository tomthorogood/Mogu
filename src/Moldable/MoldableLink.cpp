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
#include <Types/WidgetAssembly.h>

MoldableLink::MoldableLink (WidgetAssembly* assembly)
: Moldable(assembly, MoguSyntax::anchor)
{
    __init__(assembly);
}

void MoldableLink::__init__(WidgetAssembly* assembly)
{
    NodeValue v;
    __assembly_location = (std::string)
        assembly->attrdict[MoguSyntax::location.integer];
    __assembly_text = (std::string)
        assembly->attrdict[MoguSyntax::text.integer];
    initializeLink();
}

void MoldableLink::initializeLink()
{
    mApp;
    NodeValue v;
    app->interpreter().giveInput(__assembly_location,v);
    std::string href = stripquotes(v.getString());
    app->interpreter().giveInput(__assembly_text,v);
    std::string text = stripquotes(v.getString());
    if (__link)
    {
        removeWidget(__link);
        delete __link;
    }
    __link = new Wt::WAnchor(href,text);
    __link->setTarget(Wt::TargetNewWindow);
    addWidget(__link);
}
