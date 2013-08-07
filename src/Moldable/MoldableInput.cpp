/*
 * Molable_Input.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#include "Molable_Input.h"
#include <Validators/Validators.h>
#include <Mogu.h>
#include <Redis/NodeEditor.h>
#include <Types/WidgetAssembly.h>

Molable_Input::Molable_Input (WidgetAssembly* assembly)
: Moldable(assembly, MoguSyntax::input)
{
    init(assembly);
}

void Molable_Input::init(WidgetAssembly* assembly)
{
    assembly_txt = (std::string) assembly->attrdict[MoguSyntax::text.integer];
    assembly_validator = (std::string)
        assembly->attrdict[MoguSyntax::validator.integer];
    initialize_input();
}

void Molable_Input::initialize_input()
{
    if (input)
    {
        removeWidget(input);
        delete input;
        input=nullptr;
    }
    std::string txt {stripquotes(assembly_txt)};
    std::string validator {stripquotes(assembly_validator)};

    input = new Wt::WLineEdit();
    input->setEmptyText(txt);

    if (!validator.empty() && (validator_ptr))
    {
        validator_ptr = Validators::create_validator(validator);
        input->setValidator(validator_ptr);
        input->keyWentUp().connect(this, &Molable_Input::validate);
    }
    addWidget(input);
}
