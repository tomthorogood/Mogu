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
: Moldable(node, MoguSyntax::text)
{
    __init__();
}

void MoldableText::__init__()
{
    mApp;
    NodeValue v;
    Redis::ContextQuery db(Prefix::widgets);
    __text = new Wt::WText;

    app->interpreter().giveInput(getParameter(db,MoguSyntax::text), v);
    const std::string& content = v.getString();
    if (content != EMPTY)
    {
        std::string txt = stripquotes(v.getString());
        __text->setText(Wt::WString(stripquotes(v.getString()), Wt::UTF8));
    }
    addWidget(__text);
}

