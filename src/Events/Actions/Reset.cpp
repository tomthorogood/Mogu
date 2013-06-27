#include "../Actions.h"
#include "Includes.h"

namespace Actions {

void reset(Moldable& broadcaster, CommandValue& v)
{
    if ((MoguSyntax) v.get(CommandFlags::OBJECT) == MoguSyntax::own)
    {
        broadcaster.reset();
    }
    else if ((MoguSyntax) v.get(CommandFlags::OBJECT) == MoguSyntax::widget)
    {
        mApp;
        Moldable* widget = app->registeredWidget(v.getIdentifier());
        widget->reset();
    }
}

}//namespace Actions
