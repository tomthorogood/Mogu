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
    init(assembly);
}

void MoldableLink::init(WidgetAssembly* assembly)
{
    NodeValue v;
    assembly_location = (std::string)
        assembly->attrdict[MoguSyntax::location.integer];
    assembly_text = (std::string)
        assembly->attrdict[MoguSyntax::text.integer];
    initializeLink();
}

void MoldableLink::initializeLink()
{
    mApp;
    NodeValue v;
    app->interpreter().giveInput(assembly_location,v);
    std::string href = stripquotes(v.getString());
    app->interpreter().giveInput(assembly_text,v);
    std::string text = stripquotes(v.getString());
    if (link)
    {
        removeWidget(link);
        delete link;
    }
    link = new Wt::WAnchor(href,text);
    link->setTarget(Wt::TargetNewWindow);
    addWidget(link);
}
