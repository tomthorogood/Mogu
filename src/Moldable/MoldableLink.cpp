/*
 * Moldable_Link.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#include "MoldableLink.h"
#include <Mogu.h>
#include <Redis/NodeEditor.h>
#include <Types/WidgetAssembly.h>
#include "../utilities.h"

Moldable_Link::Moldable_Link (Widget_Assembly& assembly)
: Moldable(assembly, Mogu_Syntax::anchor)
{
    init(assembly);
}

void Moldable_Link::init(Widget_Assembly& assembly)
{
    assembly_location = (std::string)
        assembly.attrdict[Mogu_Syntax::location.integer];
    assembly_text = (std::string)
        assembly.attrdict[Mogu_Syntax::text.integer];
    initialize_link();
}

void Moldable_Link::initialize_link()
{
    std::string href {stripquotes(assembly_location)};
    std::string text {stripquotes(assembly_text)};
    if (link)
    {
        removeWidget(link);
        delete link;
        link = nullptr;
    }
    link = new Wt::WAnchor(href,text);
    link->setTarget(Wt::TargetNewWindow);
    addWidget(link);
}
