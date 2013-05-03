/*
 * Validators.cpp
 *
 *  Created on: Aug 14, 2012
 *      Author: tom
 */

#include "Validators.h"

#include <Exceptions/Exceptions.h>
#include <Mogu.h>
#include <Redis/ContextQuery.h>
#include <Types/NodeValue.h>
#include <Types/syntax.h>
#include <Wt/WApplication>
#include <Wt/WValidator>
#include <Wt/WRegExpValidator>

namespace Validators {


Wt::WValidator* createValidator(
    const std::string& validatorName)
{
    mApp;
    const char* c_node = validatorName.c_str();
    Redis::ContextQuery db(Prefix::validators);
    CreateQuery(db, "hget validators.%s %d", c_node, MoguSyntax::type);
    
    NodeValue vval;
    app->interpreter().giveInput(db.yieldResponse<std::string>(), vval);
    switch ((MoguSyntax) vval.getInt())
    {
        case MoguSyntax::regex:
            return createRegexValidator(db,c_node);
        default:break;
    }
    return nullptr;
}

Wt::WRegExpValidator* createRegexValidator(
    Redis::ContextQuery& db, const char* c_node)
{
    CreateQuery(db, "hget validators.%s %d", c_node, (int) MoguSyntax::test);

    std::string pattern = db.yieldResponse <std::string>();
    Wt::WString wpattern(pattern);
    return new Wt::WRegExpValidator(wpattern);
}

}    //namespace Validators

