/*
 * MoldableInput.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#include "MoldableInput.h"
#include <Validators/Validators.h>


MoldableInput::MoldableInput (const std::string& node)
: Moldable(node)
{
    __input = new Wt::WLineEdit();
    std::string param = getParameter("contents");
    __input->setEmptyText(param);
    addWidget(__input);

    param = getParameter("validator");
    if (param != EMPTY)
    {
        Wt::WValidator* validator = Validators::createValidator(param);
        __input->setValidator(validator);
        __input->keyWentUp().connect(this, &MoldableInput::validate);
    }
}
