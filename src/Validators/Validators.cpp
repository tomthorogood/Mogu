/*
 * Validators.cpp
 *
 *  Created on: Aug 14, 2012
 *      Author: tom
 */

#include "Validators.h"

#include <Exceptions/Exceptions.h>
#include <Mogu.h>
#include <Redis/MoguQueryHandler.h>
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
    Redis::MoguQueryHandler db(Application::contextMap, Prefix::validators);
    db.appendQuery( "hget validators.%s %d", c_node, (int)MoguSyntax::type);
    
    NodeValue vval;
    app->interpreter().giveInput(db.yieldResponse<std::string>(), vval);
    switch(MoguSyntax::get((int)vval))
    {
        case MoguSyntax::regex:
            return createRegexValidator(db,c_node);
        default:break;
    }
    return nullptr;
}

Wt::WRegExpValidator* createRegexValidator(
    Redis::MoguQueryHandler& db, const char* c_node)
{
    db.appendQuery( "hget validators.%s %d", c_node, (int) MoguSyntax::test);

    std::string pattern = db.yieldResponse <std::string>();
    Wt::WString wpattern(stripquotes(pattern));
    return new Wt::WRegExpValidator(wpattern);
}

}    //namespace Validators

