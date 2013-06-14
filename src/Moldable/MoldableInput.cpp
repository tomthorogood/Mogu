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
: Moldable(node, MoguSyntax::input)
{
    __init__();
}

void MoldableInput::__init__()
{
    mApp;
    NodeValue v;
    Redis::ContextQuery db(Prefix::widgets);

    __input = new Wt::WLineEdit();

    app->interpreter().giveInput(getParameter(db, MoguSyntax::text),v);
    __input->setEmptyText(v.getString());
    addWidget(__input);

    std::string param = getParameter(db,MoguSyntax::validator);;
    if (param != EMPTY)
    {
        app->interpreter().giveInput(param,v);
        Wt::WValidator* validator = Validators::createValidator(v.getString());
        __input->setValidator(validator);
        __input->keyWentUp().connect(this, &MoldableInput::validate);
    }
}
