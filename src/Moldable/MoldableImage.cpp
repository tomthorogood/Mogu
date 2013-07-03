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

MoldableImage::MoldableImage (const std::string& node)
: Moldable(node, MoguSyntax::image)
{
    __init__();
}

void MoldableImage::__init__()
{
    mApp;
    NodeValue v;
    Redis::NodeEditor node(Prefix::widgets, __node);
    initializeNodeEditor(node);
    std::string param = getParameter(node, MoguSyntax::source);
    app->interpreter().giveInput(param,v);
    std::string src = stripquotes(v.getString());

    param = getParameter(node, MoguSyntax::text);
    app->interpreter().giveInput(param,v);
    std::string alt = stripquotes(v.getString());
    __image = new Wt::WImage(src, alt);
    addWidget(__image);
//!TODO Re-add properties
//    if (hasProperty("scaled"))
//    {
//        __image->addStyleClass("mogu_SCALED_IMAGE");
//    }
}
