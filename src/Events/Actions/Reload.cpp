#include "../Actions.h"
#include "Includes.h"

namespace Actions {

void reload(Moldable& broadcaster, CommandValue& v)
{
    switch(v.getObject())
    {
        case MoguSyntax::own:{
            broadcaster.reload();
            break;}
        case MoguSyntax::widget:{
            mApp;
            Moldable* widget = app->registeredWidget(v.getIdentifier());
            widget->reload();
            break;}
        default: return;
    }
}

}//namespace Actions
