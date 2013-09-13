#include "../Actions.h"
#include "../../Types/GroupManager.h"
#include "Includes.h"

#include <Redis/NodeEditor.h>

namespace Actions
{
void emit(Moldable& broadcaster, Command_Value& v)
{
    mApp;
    const Syntax_Def& o {Mogu_Syntax::get(v.get(Command_Flags::object))};
    Moldable* emitter
    {
        o == Mogu_Syntax::own
            ?  &broadcaster : 
        o == Mogu_Syntax::widget
            ? app->get_widget(
                v.get(Command_Flags::identifier)) :
            nullptr
    };

    switch(v.get(Command_Flags::arg))
    {
        case Mogu_Syntax::style_changed:
            emitter->styleChanged().emit();
            break;
        case Mogu_Syntax::succeed:
            emitter->succeed().emit();
            break;
        case Mogu_Syntax::onload:
            emitter->onLoad().emit();
            break;
        case Mogu_Syntax::hidden_changed:
            emitter->hiddenChanged().emit();
            break;
        case Mogu_Syntax::index_changed:
            emitter->indexChanged().emit();
            break;
        case Mogu_Syntax::click:
        {
            Wt::JavaScriptEvent e {}; 
            e.button=1;
            Wt::WMouseEvent m{e};
            emitter->clicked().emit(m);
            break;
        }
        case Mogu_Syntax::error_reported:
            emitter->errorReported().emit();
            break;
            break;
        case Mogu_Syntax::enter_pressed:
            emitter->enterPressed().emit();
            break;
        default: break;
    }
}

}
