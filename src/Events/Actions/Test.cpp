#include "../Actions.h"
#include "Includes.h"
#include <Types/MoguLogger.h>

namespace Actions {

void test(Moldable& broadcaster, CommandValue& v)
{
    mApp;

    bool result = false;
    NodeValue value;

    // This is a little bit of reverse engineering, because it undoes some of
    // tokenization that's already been done, which is inefficient, but it's
    // much easier to understand than the previous implementation.
    std::string state_str = v.stitchState();

    app->interpreter().giveInput(state_str, value);
    if (value.isString()) {
        value.setString(stripquotes(value.getString()));
    }
    if (v.get(CommandFlags::VALUE).isString()) {
        NodeValue tmp = v.get(CommandFlags::VALUE);
        tmp.setString(stripquotes(tmp.getString()));
        v.set(CommandFlags::VALUE, tmp);
    }
    result = (stripquotes(value) == stripquotes(v.get(CommandFlags::VALUE)));

    Application::log.log(LogLevel::NOTICE
            , (std::string) value, " == "
            , (std::string) v.get(CommandFlags::VALUE)
            , "? "
            , result ? "SUCCEEDED" : "FAILED");
    if (!result) broadcaster.fail().emit();
    else broadcaster.succeed().emit();

}

}//namespace
