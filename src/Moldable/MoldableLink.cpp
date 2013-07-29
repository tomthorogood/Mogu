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
    assembly_location = (std::string)
        assembly->attrdict[MoguSyntax::location.integer];
    assembly_text = (std::string)
        assembly->attrdict[MoguSyntax::text.integer];
    initializeLink();
}

void MoldableLink::initializeLink()
{
    std::string href = stripquotes(assembly_location);
    std::string text = stripquotes(assembly_text);
    if (link != nullptr)
    {
        removeWidget(link);
        delete link;
    }
    link = new Wt::WAnchor(href,text);
    link->setTarget(Wt::TargetNewWindow);
    addWidget(link);
}
