/*
 * Moldable_Link.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#include "Moldable_Link.h"
#include <Mogu.h>
#include <Redis/NodeEditor.h>
#include <Types/Widget_Assembly.h>

Moldable_Link::Moldable_Link (Widget_Assembly* assembly)
: Moldable(assembly, MoguSyntax::anchor)
{
    init(assembly);
}

void Moldable_Link::init(Widget_Assembly* assembly)
{
    assembly_location = (std::string)
        assembly->attrdict[MoguSyntax::location.integer];
    assembly_text = (std::string)
        assembly->attrdict[MoguSyntax::text.integer];
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
