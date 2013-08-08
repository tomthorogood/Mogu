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
#include <Types/Widget_Assembly.h>

Molable_Input::Molable_Input (Widget_Assembly* assembly)
: Moldable(assembly, Mogu_Syntax::input)
{
    init(assembly);
}

void Molable_Input::init(Widget_Assembly* assembly)
{
    assembly_txt = (std::string) assembly->attrdict[Mogu_Syntax::text.integer];
    assembly_validator = (std::string)
        assembly->attrdict[Mogu_Syntax::validator.integer];
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
