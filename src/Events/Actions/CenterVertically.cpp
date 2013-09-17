/* CenterVertically.cpp
 *
 * Created on September 16 2013
 * Author: Tom Thorogood
 */

#include "../Actions.h"
#include <Types/CommandValue.h>
#include <Mogu.h>

namespace Actions
{
void center_vertically(Moldable& bc, Command_Value& v)
{
    const Syntax_Def& o {Mogu_Syntax::get(v.get(Command_Flags::object))};
    switch(o)
    {
        case Mogu_Syntax::own:
            bc.center_vertically();
            break;
        case Mogu_Syntax::widget:
        {
            mApp;
            Moldable* w {app->get_widget(v.get_identifier())};
            if (w) w->center_vertically();
            break;
        }
        default:break;
    };
}
}
