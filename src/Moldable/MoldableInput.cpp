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
    init(assembly);
}

void MoldableInput::init(WidgetAssembly* assembly)
{
    assembly_txt = (std::string)
        assembly->attrdict[MoguSyntax::text.integer];
    assembly_validator = (std::string)
        assembly->attrdict[MoguSyntax::validator.integer];
    initializeInput();
}

void MoldableInput::initializeInput()
{
    if (input)
    {
        removeWidget(input);
        delete input;
    }
    mApp;
    std::string txt = stripquotes(assembly_txt);
    std::string validator = stripquotes(assembly_validator);
    input = new Wt::WLineEdit();
    input->setEmptyText(txt);
    if (!validator.empty() && !validator_ptr)
    {
        validator_ptr = Validators::createValidator(validator);
        input->setValidator(validator_ptr);
        input->keyWentUp().connect(this, &MoldableInput::validate);
    }
}
