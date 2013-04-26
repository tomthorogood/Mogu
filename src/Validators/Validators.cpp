/*
 * Validators.cpp
 *
 *  Created on: Aug 14, 2012
 *      Author: tom
 */

#include <Validators/Validators.h>
#include <Wt/WValidator>
#include <Wt/WRegExpValidator>
#include <Redis/RedisCore.h>
#include <Types/NodeValue.h>
#include <Wt/WApplication>
#include <Exceptions/Exceptions.h>
#include <Mogu.h>
#include <Redis/ContextQuery.h>
#include <Types/syntax.h>
namespace Validators {

using std::string;

Wt::WValidator* createValidator(
    const std::string& validatorName)
{
    mApp;
    const char* c_node = validatorName.c_str();
    Redis::ContextQuery db(Prefix::validators);
    CreateQuery(db,
        new Redis::Query("hget validators.%s %d", c_node, MoguSyntax::type));
;
    NodeValue vval;
    app->interpreter().giveInput(db.yieldResponse<std::string>(), vval);
    switch (vval.getInt())
    {
        case MoguSyntax::regex:
          return createRegexValidator(db,c_node);
    }
    return nullptr;
}

Wt::WRegExpValidator* createRegexValidator(
    Redis::ContextQuery& db, const char* c_node)
{
    CreateQuery(db,
        new Redis::Query("hget validators.%s %d", c_node, MoguSyntax::test));

    std::string pattern = db.yieldResponse <std::string>();
    Wt::WString wpattern(pattern);
    return new Wt::WRegExpValidator(wpattern);
}

}    //namespace Validators

