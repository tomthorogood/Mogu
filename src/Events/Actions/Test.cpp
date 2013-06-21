#include "../Actions.h"
#include "Includes.h"

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
    result = (value == v.get(CommandFlags::VALUE));
#ifdef DEBUG
    std::string s_res = result ? "SUCCEEDED" : "FAILED";
    std::cout << (std::string) value << " == "
        << (std::string) v.get(CommandFlags::VALUE)
        << "? " << s_res << std::endl;
#endif
    if (!result) broadcaster.fail().emit();
    else broadcaster.succeed().emit();

}

}//namespace
