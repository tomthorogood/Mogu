/*
 * MoldableInput.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#include "MoldableInput.h"
#include <Validators/Validators.h>
#include <Mogu.h>

MoldableInput::MoldableInput (const std::string& node)
: Moldable(node)
{
    mApp;
    NodeValue v;
    __input = new Wt::WLineEdit();
    std::string param = getParameter("content");
    app->interpreter().giveInput(param,v);
    __input->setEmptyText(v.getString());
    addWidget(__input);

    param = getParameter("validator");
    if (param != EMPTY)
    {
        app->interpreter().giveInput(param,v);
        Wt::WValidator* validator = Validators::createValidator(v.getString());
        __input->setValidator(validator);
        __input->keyWentUp().connect(this, &MoldableInput::validate);
    }
}
