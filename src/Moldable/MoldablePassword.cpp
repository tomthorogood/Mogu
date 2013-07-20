/*
 * MoldablePassword.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#include "MoldablePassword.h"
#include <Types/WidgetAssembly.h>

MoldablePassword::MoldablePassword(WidgetAssembly* assembly)
: MoldableInput(assembly)
{
    input->setEchoMode(Wt::WLineEdit::Password);
}


