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
    NodeValue v;
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
    NodeValue nv_src;
    NodeValue nv_txt;
    mApp;
    app->interpreter().giveInput(assembly_src,nv_src);
    app->interpreter().giveInput(assembly_txt,nv_txt);
    std::string src = stripquotes((std::string)nv_src);
    std::string txt = stripquotes((std::string)nv_txt);
    image = new Wt::WImage(src,txt);
    addWidget(image);
}
