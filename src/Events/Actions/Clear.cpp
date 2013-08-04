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

void clear(Moldable& broadcaster, CommandValue& v)
{
    switch(MoguSyntax::get(v.get(CommandFlags::OBJECT)).integer)
    {
    case MoguSyntax::own:
        broadcaster.clear();
        break;
    case MoguSyntax::widget:
    {
        mApp;
        Moldable* w = app->registeredWidget(v.getIdentifier());
        if (w) w->clear();
        break;
    }
    default: return;

    }
}

}

