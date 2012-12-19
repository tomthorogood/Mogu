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
#include <Types/syntax.h>
namespace Validators {

using std::string;

Wt::WValidator* createValidator(
    std::string validatorName)
{
    mApp;
    Wt::WValidator* validator = 0;
    string validator_node = "validators." + validatorName;
    app->redisExec(Mogu::Keep, "hget %s type", validator_node.c_str());
    string vtype =  redisReply_STRING;
    if (vtype == EMPTY) throw Exceptions::Err_MissingProperty(vtype, "type");
    NodeValue vval;
    app->interpreter().giveInput(vtype, vval);
    switch (vval.getInt()) {
    case Tokens::regex: {
        validator = (Wt::WValidator*) createRegexValidator(validator_node);
        break;
    }
    }
    return validator;
}

Wt::WRegExpValidator* createRegexValidator(
    string node)
{
    mApp;
    Wt::WRegExpValidator* validator = 0;
    app->redisExec(Mogu::Keep, "hget %s test", node.c_str());
    string pattern = redisReply_STRING;
    if (pattern == EMPTY) throw Exceptions::Err_MissingProperty(node, "test");
    Wt::WString wpattern(pattern);
    validator = new Wt::WRegExpValidator(wpattern);
    return validator;
}

}    //namespace Validators

