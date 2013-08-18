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
    const char* c_node {validator_name.c_str()};
    Redis::Mogu_Query_Handler db {Prefix::validators};
    db.append_query( "hget validators.%s %d", c_node, (int)Mogu_Syntax::type);
    
    Node_Value v {};
    app->get_interpreter().give_input(db.yield_response<std::string>(), v);
    switch(Mogu_Syntax::get(v.get_int()))
    {
        case Mogu_Syntax::regex:
            return create_regex_validator(db,c_node);
        default:break;
    }
    return nullptr;
}

Wt::WRegExpValidator* create_regex_validator(
    Redis::Mogu_Query_Handler& db, const char* c_node)
{
    db.append_query( "hget validators.%s %d", c_node, (int) Mogu_Syntax::test);
    std::string pattern {db.yield_response <std::string>()};
    Wt::WString w {stripquotes(pattern)};
    return new Wt::WRegExpValidator {w};
}

}    //namespace Validators

