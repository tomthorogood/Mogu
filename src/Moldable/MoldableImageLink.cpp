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
    init(assembly);
}

void MoldableImageLink::init(WidgetAssembly* assembly)
{
    assembly_src = (std::string)
        assembly->attrdict[MoguSyntax::source.integer];
    initializeImage();
}

void MoldableImageLink::initializeImage()
{
    if (image)
    {
        removeWidget(image);
        delete image;
    }
    mApp;
    NodeValue nv_src;
    app->interpreter().giveInput(assembly_src,nv_src);
    std::string src = stripquotes(nv_src.getString());
    image = new Wt::WImage(src, moldableValue());
    link->setImage(image);
    link->setTarget(Wt::TargetNewWindow);
}

