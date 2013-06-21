#include "../Actions.h"
#include "Includes.h"

namespace Actions {

void reset(Moldable& broadcaster, CommandValue& v)
{
    if (v.getObject() == MoguSyntax::own)
    {
        broadcaster.reset();
    }
    else if (v.getObject() == MoguSyntax::widget)
    {
        mApp;
        Moldable* widget = app->registeredWidget(v.getIdentifier());
        widget->reset();
    }
}

}//namespace Actions
