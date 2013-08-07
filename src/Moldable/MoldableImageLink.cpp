/*
 * Moldable_Image_Link.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#include "Moldable_Image_Link.h"
#include <Mogu.h>
#include <Wt/WAnchor>
#include <Wt/WImage>
#include <Redis/NodeEditor.h>
#include <Types/WidgetAssembly.h>

Moldable_Image_Link::Moldable_Image_Link(WidgetAssembly* assembly)
: MoldableLink(assembly)
{
    init(assembly);
}

void Moldable_Image_Link::init(WidgetAssembly* assembly)
{
    assembly_src = (std::string) assembly->attrdict[MoguSyntax::source.integer];
    initializeImage();
}

void Moldable_Image_Link::initializeImage()
{
    if (image)
    {
        removeWidget(image);
        delete image;
        image = nullptr;
    }
    std::string src = stripquotes(assembly_src);
    image = new Wt::WImage(src, moldableValue());
    link->setImage(image);
    link->setTarget(Wt::TargetNewWindow);
}

