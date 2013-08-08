#include "../Actions.h"
#include "Includes.h"

namespace Actions {

void reload(Moldable& broadcaster, Command_Value& v)
{
    int syn {(int) Mogu_Syntax::get(v.get(Command_Flags::object))};
    switch(syn)
    {
        case (int) Mogu_Syntax::own:{
            broadcaster.reload();
            break;}
        case (int) Mogu_Syntax::widget:{
            mApp;
            Moldable* widget = app->get_widget(v.get_identifier());
            if (widget) widget->reload();
            break;}
        default: return;
    }
}

}//namespace Actions
