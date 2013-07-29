/*
 * MoldableImage.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#include "MoldableImage.h"
#include <Redis/ContextQuery.h>
#include <Parsers/NodeValueParser.h>
#include <Redis/NodeEditor.h>
#include <Mogu.h>
#include <Types/WidgetAssembly.h>

MoldableImage::MoldableImage (WidgetAssembly* assembly)
: Moldable(assembly, MoguSyntax::image)
{
    init(assembly);
}

void MoldableImage::init(WidgetAssembly* assembly)
{
    assembly_src = (std::string)
        assembly->attrdict[MoguSyntax::source.integer];

    assembly_txt = (std::string)
        assembly->attrdict[MoguSyntax::text.integer];
    initializeImage();
//!TODO Re-add properties
//    if (hasProperty("scaled"))
//    {
//        image->addStyleClass("mogu_SCALED_IMAGE");
//    }
}

void MoldableImage::initializeImage()
{
    if (image != NULL)
    {
        removeWidget(image);
        delete image;
    }

    std::string src = stripquotes(assembly_src);
    std::string txt = stripquotes(assembly_txt);
    image = new Wt::WImage(src,txt);
    addWidget(image);
}
