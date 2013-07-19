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
    __init__(assembly);
}

void MoldableImage::__init__(WidgetAssembly* assembly)
{
    NodeValue v;
    __assembly_src = (std::string)
        assembly->attrdict[MoguSyntax::source.integer];

    __assembly_txt = (std::string)
        assembly->attrdict[MoguSyntax::text.integer];
    initializeImage();
//!TODO Re-add properties
//    if (hasProperty("scaled"))
//    {
//        __image->addStyleClass("mogu_SCALED_IMAGE");
//    }
}

void MoldableImage::initializeImage()
{
    if (__image)
    {
        removeWidget(__image);
        delete __image;
    }
    NodeValue nv_src;
    NodeValue nv_txt;
    mApp;
    app->interpreter().giveInput(__assembly_src,nv_src);
    app->interpreter().giveInput(__assembly_txt,nv_txt);
    std::string src = stripquotes((std::string)nv_src);
    std::string txt = stripquotes((std::string)nv_txt);
    __image = new Wt::WImage(src,txt);
    addWidget(__image);
}
