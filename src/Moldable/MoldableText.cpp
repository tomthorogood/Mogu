/*
 * MoldableText.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#include "MoldableText.h"
#include <Redis/ContextQuery.h>
#include <Mogu.h>

MoldableText::MoldableText(const std::string& node)
: Moldable(node)
{
    __init__();
}

void MoldableText::__init__()
{
    Moldable::__init__();
    mApp;
    NodeValue v;
    Redis::ContextQuery db(Prefix::widgets);
    __text = new Wt::WText;

    app->interpreter().giveInput(getParameter(db,MoguSyntax::text), v);
    const std::string& content = v.getString();
    if (content != EMPTY)
    {
        __text->setText(v.getString());
    }
    addWidget(__text);
}

