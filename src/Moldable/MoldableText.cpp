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
    init(assembly);
}

void MoldableText::init(WidgetAssembly* assembly)
{
    NodeValue v;
    assembly_text = (std::string)
        assembly->attrdict[MoguSyntax::text.integer];
    text = new Wt::WText;
    initializeText();
    addWidget(text);
}

void MoldableText::initializeText()
{
    mApp;
    NodeValue v;
    app->interpreter().giveInput(assembly_text,v);
    const std::string& content = v.getString();
    if (!content.empty())
    {
        std::string txt = stripquotes(content);
        text->setText(Wt::WString(txt,Wt::UTF8));
    }
}
