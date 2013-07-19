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
#include <Types/WidgetAssembly.h>

MoldableInput::MoldableInput (WidgetAssembly* assembly)
: Moldable(assembly, MoguSyntax::input)
{
    __init__(assembly);
}

void MoldableInput::__init__(WidgetAssembly* assembly)
{
    __assembly_txt = (std::string)
        assembly->attrdict[MoguSyntax::text.integer];
    __assembly_validator = (std::string)
        assembly->attrdict[MoguSyntax::validator.integer];
    initializeInput();
}

void MoldableInput::initializeInput()
{
    if (__input)
    {
        removeWidget(__input);
        delete __input;
    }
    mApp;
    NodeValue nv;
    app->interpreter().giveInput(__assembly_txt,nv);
    std::string txt = stripquotes(nv.getString());
    app->interpreter().giveInput(__assembly_validator,nv);
    std::string validator = stripquotes(nv.getString());
    __input = new Wt::WLineEdit();
    __input->setEmptyText(txt);
    if (!validator.empty() && !__validator)
    {
        __validator = Validators::createValidator(validator);
        __input->setValidator(__validator);
        __input->keyWentUp().connect(this, &MoldableInput::validate);
    }
}
