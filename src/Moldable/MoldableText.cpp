/*
 * Moldable_Text.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#include "MoldableText.h"
#include <Mogu.h>
#include <Redis/NodeEditor.h>
#include <Types/WidgetAssembly.h>
#include "../utilities.h"

Moldable_Text::Moldable_Text(Widget_Assembly& assembly) :
    Moldable(assembly, Mogu_Syntax::text)
{
    init(assembly);
}

void Moldable_Text::init(Widget_Assembly& assembly)
{
    assembly_text = (std::string) assembly.attrdict[Mogu_Syntax::text.integer];
    text = new Wt::WText;
    initialize_text();
    addWidget(text);
}

void Moldable_Text::initialize_text()
{
    if (!assembly_text.empty())
    {
        std::string txt {stripquotes(assembly_text)};
        text->setText(Wt::WString(txt,Wt::UTF8));
    }
}
