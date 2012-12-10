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
    std::string src = getParameter("source");
    std::string alt = getParameter("contents");
    __image = new Wt::WImage(src, alt);
    addWidget(__image);
}
