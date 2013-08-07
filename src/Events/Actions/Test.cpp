#include "../Actions.h"
#include "Includes.h"
#include <Types/MoguLogger.h>

namespace Actions {

void test(Moldable& broadcaster, CommandValue& v)
{
    mApp;

    bool result {false};
    NodeValue value {};

    // This is a little bit of reverse engineering, because it undoes some of
    // tokenization that's already been done, which is inefficient, but it's
    // much easier to understand than the previous implementation.
    std::string state = v.join_state();

    app->interpreter().give_input(state, value);
    if (value.is_string())
        value.set_string(stripquotes(value.get_string()));

    if (v.get(Command_Flags::value).is_string())
    {
        Node_Value tmp {v.get(Command_Flags::value)};
        tmp.set_string(tmp.get_string());
        v.set(Command_Flags::value,tmp);
    }

    result = (stripquotes(value) == stripquotes(v.get(CommandFlags::VALUE)));

    Application::log.log(Log_Level::notice
            , (std::string) value, " == "
            , (std::string) v.get(CommandFlags::VALUE)
            , "? "
            , result ? "SUCCEEDED" : "FAILED");
    if (!result) broadcaster.fail().emit();
    else broadcaster.succeed().emit();
}

}//namespace
