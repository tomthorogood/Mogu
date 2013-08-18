/*
 * Clear.cpp
 *
 *  Created on: Aug 3, 2013
 *      Author: tom
 */


#include "../Actions.h"
#include <Types/CommandValue.h>
#include <Mogu.h>

namespace Actions
{

void clear(Moldable& broadcaster, Command_Value& v)
{
    switch(Mogu_Syntax::get(v.get(Command_Flags::object)).integer)
    {
    case Mogu_Syntax::own:
        broadcaster.clear();
        break;
    case Mogu_Syntax::widget:
    {
        mApp;
        Moldable* w = app->get_widget(v.get_identifier());
        if (w) w->clear();
        break;
    }
    default: return;

    }
}

}

