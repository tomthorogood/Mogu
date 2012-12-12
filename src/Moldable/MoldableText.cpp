/*
 * MoldableText.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#include "MoldableText.h"
#include <Mogu.h>

MoldableText::MoldableText(const std::string& node)
: Moldable(node)
{
    __init__();
}

void MoldableText::__init__()
{
    //Moldable::__init__();
    mApp;
    __text = new Wt::WText;
    std::string param = getParameter("content");
    NodeValue v;
    app->interpreter().giveInput(param, v);
    if (v.getString() != EMPTY)
    {
        __text->setText(v.getString());
    }
    addWidget(__text);
}

