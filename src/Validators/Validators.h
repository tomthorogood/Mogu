/*
 * ValidatorParser.h
 *
 *  Created on: Aug 14, 2012
 *      Author: tom
 */

#ifndef VALIDATORPARSER_H_
#define VALIDATORPARSER_H_

#include <Wt/WValidator>
namespace Validators {

Wt::WValidator* create_validator(const std::string& validator);
Wt::WRegExpValidator* create_regex_validator(
        Redis::MoguQueryHandler&,const char*);

}    //namespace Validators

#endif /* VALIDATORPARSER_H_ */
