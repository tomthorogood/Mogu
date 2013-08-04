#include "../Actions.h"
#include "Includes.h"

namespace Actions {

void reload(Moldable& broadcaster, CommandValue& v)
{
    int syn = (int) MoguSyntax::get(v.get(CommandFlags::OBJECT));
    switch(syn)
    {
        case (int) MoguSyntax::own:{
            broadcaster.reload();
            break;}
        case (int) MoguSyntax::widget:{
            mApp;
            Moldable* widget = app->registeredWidget(v.getIdentifier());
            if (widget) widget->reload();
            break;}
        default: return;
    }
}

}//namespace Actions
