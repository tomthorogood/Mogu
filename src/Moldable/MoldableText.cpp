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
    if (!assembly_text.empty())
    {
        std::string txt = stripquotes(assembly_text);
        text->setText(Wt::WString(txt,Wt::UTF8));
    }
}
