/*
 * Moldable_Image_Link.cpp
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
#include "../Config/inline_utils.h"

Moldable_Image_Link::Moldable_Image_Link(Widget_Assembly& assembly)
: Moldable_Link(assembly)
{
    init(assembly);
}

void Moldable_Image_Link::init(Widget_Assembly& assembly)
{
    assembly_src = (std::string) assembly.attrdict[Mogu_Syntax::source.integer];
    initialize_image();
}

void Moldable_Image_Link::initialize_image()
{
    if (image)
    {
        removeWidget(image);
        delete image;
        image = nullptr;
    }
    std::string src = stripquotes(assembly_src);
    image = new Wt::WImage(src, get_moldable_value());
    link->setImage(image);
    link->setTarget(Wt::TargetNewWindow);
}

