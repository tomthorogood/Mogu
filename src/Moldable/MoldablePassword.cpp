/*
 * MoldablePassword.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#include "MoldablePassword.h"

MoldablePassword::MoldablePassword(const std::string& node)
: MoldableInput(node)
{
    __input->setEchoMode(Wt::WLineEdit::Password);
}


