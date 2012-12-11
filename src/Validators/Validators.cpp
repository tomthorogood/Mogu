/*
 * Validators.cpp
 *
 *  Created on: Aug 14, 2012
 *      Author: tom
 */

#include <Validators/Validators.h>

#include <Types/Enums.h>
#include <Parsers/StyleParser.h>
#include <Parsers/Parsers.h>
#include <Wt/WValidator>
#include <Wt/WRegExpValidator>
#include <Redis/RedisCore.h>
#include <Types/NodeValue.h>
#include <Wt/WApplication>
#include <Exceptions/Exceptions.h>
namespace Validators {

using namespace Parsers::StyleParser;
using namespace Enums::Validators;
using Parsers::ValidatorTypeParser;
using std::string;

Wt::WValidator* createValidator(
    std::string validatorName)
{
    mApp;
    Wt::WValidator* validator = 0;
    string validator_node = "validators." + validatorName;
    string vtype = Parsers::StyleParser::getHashEntry(validator_node, "type");
    if (vtype == EMPTY) throw Exceptions::Err_MissingProperty(vtype, "type");
    NodeValue vval;
    app->interpreter().giveInput(vtype, vval, NULL,
        Parsers::enum_callback<Parsers::ValidatorTypeParser>);
    switch (vval.getInt()) {
    case regex: {
        validator = (Wt::WValidator*) createRegexValidator(validator_node);
        break;
    }
    }
    return validator;
}

Wt::WRegExpValidator* createRegexValidator(
    string node)
{
    Wt::WRegExpValidator* validator = 0;
    string pattern = Parsers::StyleParser::getHashEntry(node, "test");
    if (pattern == EMPTY) throw Exceptions::Err_MissingProperty(node, "test");
    Wt::WString wpattern(pattern);
    validator = new Wt::WRegExpValidator(wpattern);
    return validator;
}

}    //namespace Validators

