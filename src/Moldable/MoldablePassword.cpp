/*
 * Moldable_Password.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#include "MoldablePassword.h"
#include <Types/WidgetAssembly.h>

Moldable_Password::Moldable_Password(Widget_Assembly& assembly)
: Moldable_Input(assembly)
{
    input->setEchoMode(Wt::WLineEdit::Password);
}


