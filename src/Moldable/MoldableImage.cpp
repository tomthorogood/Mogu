/*
 * MoldableImage.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#include "MoldableImage.h"
#include <Mogu.h>

MoldableImage::MoldableImage (const std::string& node)
: Moldable(node)
{
    NodeValue v;
    mApp;

    std::string param = getParameter("source");
    app->interpreter().giveInput(param, v);
    std::string src = v.getString();

    param = getParameter("content");
    app->interpreter().giveInput(param,v);
    std::string alt = v.getString();
    __image = new Wt::WImage(src, alt);
    addWidget(__image);
}
