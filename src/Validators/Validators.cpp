/*
 * Validators.cpp
 *
 *  Created on: Aug 14, 2012
 *      Author: tom
 */

#include "Validators.h"

#include <Mogu.h>
#include <Types/NodeValue.h>
#include <Types/syntax.h>
#include <Wt/WApplication>
#include <Wt/WValidator>
#include <Wt/WRegExpValidator>
#include "../Config/inline_utils.h"


namespace Validators {


Wt::WValidator* create_validator(
    const std::string& validator_name)
{
    mApp;
    Node_Value arg {Mogu_Syntax::type.str};
    Redis::Node_Editor e {Prefix::validator, validator_name, &arg};
    std::string s {e.read()};
        Node_Value v {};
    app->get_interpreter().give_input(s, v);
    switch(Mogu_Syntax::get(v.get_int()))
    {
        case Mogu_Syntax::regex:
        {
            arg.set_string(Mogu_Syntax::test.str);
            e.swap_arg(&arg);
            std::string test {e.read()};
            return create_regex_validator(test);
        }
        default:break;
    }
    return nullptr;
}

Wt::WRegExpValidator* create_regex_validator(const std::string& pattern)
{
    Wt::WString w {stripquotes(pattern)};
    return new Wt::WRegExpValidator {w};
}

}    //namespace Validators

