/*
 * Moldable_Image.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#include "MoldableImage.h"
#include <Parsers/NodeValueParser.h>
#include <Redis/NodeEditor.h>
#include <Mogu.h>
#include <Types/WidgetAssembly.h>
#include "../utilities.h"

Moldable_Image::Moldable_Image (Widget_Assembly& assembly)
: Moldable(assembly, Mogu_Syntax::image)
{
    init(assembly);
}

void Moldable_Image::init(Widget_Assembly& assembly)
{
    assembly_src = (std::string)
        assembly.attrdict[Mogu_Syntax::source.integer];

    assembly_txt = (std::string)
        assembly.attrdict[Mogu_Syntax::text.integer];
    initialize_image();
}

void Moldable_Image::initialize_image()
{
    if (image)
    {
        removeWidget(image);
        delete image;
        image = nullptr;
    }

    std::string src {stripquotes(assembly_src)};
    std::string txt {stripquotes(assembly_txt)};
    image = new Wt::WImage(src,txt);
    addWidget(image);
}
