#include "../Actions.h"
#include "Includes.h"
#include <Types/MoguLogger.h>
#include "../../Config/inline_utils.h"

namespace Application {
    extern Mogu_Logger log;
}

namespace Actions {

void test(Moldable& broadcaster, Command_Value& v)
{
    mApp;

    bool result {false};
    Node_Value value {};

    // This is a little bit of reverse engineering, because it undoes some of
    // tokenization that's already been done, which is inefficient, but it's
    // much easier to understand than the previous implementation.
    std::string state = v.join_state();

    app->get_interpreter().give_input(state, value);
    if (value.is_string())
        value.set_string(stripquotes(value.get_string()));

    if (v.get(Command_Flags::value).is_string())
    {
        Node_Value tmp {v.get(Command_Flags::value)};
        tmp.set_string(tmp.get_string());
        v.set(Command_Flags::value,tmp);
    }

    result = (stripquotes(value) == stripquotes(v.get(Command_Flags::value)));

    Application::log.log(Log_Level::notice
            , (std::string) value, " == "
            , (std::string) v.get(Command_Flags::value)
            , "? "
            , result ? "SUCCEEDED" : "FAILED");
    if (!result) broadcaster.fail().emit();
    else broadcaster.succeed().emit();
}

}//namespace
