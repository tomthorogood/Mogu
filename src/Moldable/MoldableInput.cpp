/*
 * MoldableInput.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#include "MoldableInput.h"
#include <Validators/Validators.h>
#include <Mogu.h>
#include <Redis/NodeEditor.h>

MoldableInput::MoldableInput (const std::string& node)
: Moldable(node, MoguSyntax::input)
{
    __init__();
}

void MoldableInput::__init__()
{
    mApp;
    NodeValue v;

    Redis::NodeEditor node(Prefix::widgets, __node);
    initializeNodeEditor(node);

    __input = new Wt::WLineEdit();
    std::string strval = getParameter(node, MoguSyntax::text);
    if (strval != EMPTY)
    {
        app->interpreter().giveInput(getParameter(node, MoguSyntax::text),v);
        __input->setEmptyText(stripquotes(v.getString()));
    }
    addWidget(__input);

    std::string param = getParameter(node,MoguSyntax::validator);;
    if (param != EMPTY)
    {
        app->interpreter().giveInput(param,v);
        Wt::WValidator* validator = Validators::createValidator(v.getString());
        __input->setValidator(validator);
        __input->keyWentUp().connect(this, &MoldableInput::validate);
    }
}
