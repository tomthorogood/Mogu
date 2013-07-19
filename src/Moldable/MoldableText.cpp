/*
 * MoldableText.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#include "MoldableText.h"
#include <Redis/ContextQuery.h>
#include <Mogu.h>
#include <Redis/NodeEditor.h>
#include <Types/WidgetAssembly.h>

MoldableText::MoldableText(WidgetAssembly* assembly) :
    Moldable(assembly, MoguSyntax::text)
{
    __init__(assembly);
}

void MoldableText::__init__(WidgetAssembly* assembly)
{
    NodeValue v;
    __assembly_text = (std::string)
        assembly->attrdict[MoguSyntax::text.integer];
    __text = new Wt::WText;
    initializeText();
    addWidget(__text);
}

void MoldableText::initializeText()
{
    mApp;
    NodeValue v;
    app->interpreter().giveInput(__assembly_text,v);
    const std::string& content = v.getString();
    if (!content.empty())
    {
        std::string txt = stripquotes(content);
        __text->setText(Wt::WString(txt,Wt::UTF8));
    }
}
