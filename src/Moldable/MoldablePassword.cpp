/*
 * Moldable_Password.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#include "Moldable_Password.h"
#include <Types/Widget_Assembly.h>

Moldable_Password::Moldable_Password(Widget_Assembly* assembly)
: Moldable_Input(assembly)
{
    input->setEchoMode(Wt::WLineEdit::Password);
}


