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
#include <Types/WidgetAssembly.h>

MoldableImageLink::MoldableImageLink(WidgetAssembly* assembly)
: MoldableLink(assembly)
{
    __init__(assembly);
}

void MoldableImageLink::__init__(WidgetAssembly* assembly)
{
    __assembly_src = (std::string)
        assembly->attrdict[MoguSyntax::source.integer];
    initializeImage();
}

void MoldableImageLink::initializeImage()
{
    if (__image)
    {
        removeWidget(__image);
        delete __image;
    }
    mApp;
    NodeValue nv_src;
    app->interpreter().giveInput(__assembly_src,nv_src);
    std::string src = stripquotes(nv_src.getString());
    __image = new Wt::WImage(src, moldableValue());
    __link->setImage(__image);
    __link->setTarget(Wt::TargetNewWindow);
}

