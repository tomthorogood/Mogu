/*
 * MoldableImage.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#include "MoldableImage.h"
#include <Redis/ContextQuery.h>
#include <Parsers/NodeValueParser.h>
#include <Mogu.h>

MoldableImage::MoldableImage (const std::string& node)
: Moldable(node)
{
    __init__();
}

void MoldableImage::__init__()
{
    mApp;
    NodeValue v;
    Redis::ContextQuery db(Prefix::widgets);
    std::string param = getParameter(db, MoguSyntax::source);
    app->interpreter().giveInput(param,v);
    std::string src = v.getString();

    param = getParameter(db, MoguSyntax::text);
    app->interpreter().giveInput(param,v);
    std::string alt = v.getString();
    __image = new Wt::WImage(src, alt);
    addWidget(__image);
//!TODO Re-add properties
//    if (hasProperty("scaled"))
//    {
//        __image->addStyleClass("mogu_SCALED_IMAGE");
//    }
}
