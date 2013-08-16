/*
 * Moldable_Input.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#include "MoldableInput.h"
#include <Validators/Validators.h>
#include <Mogu.h>
#include <Redis/NodeEditor.h>
#include <Types/WidgetAssembly.h>
#include "../Config/inline_utils.h"

Moldable_Input::Moldable_Input (Widget_Assembly* assembly)
: Moldable(assembly, Mogu_Syntax::input)
{
    init(assembly);
}

void Moldable_Input::init(Widget_Assembly* assembly)
{
    assembly_txt = (std::string) assembly->attrdict[Mogu_Syntax::text.integer];
    assembly_validator = (std::string)
        assembly->attrdict[Mogu_Syntax::validator.integer];
    initialize_input();
}

void Moldable_Input::initialize_input()
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
        input->keyWentUp().connect(this, &Moldable_Input::validate);
    }
    addWidget(input);
}
