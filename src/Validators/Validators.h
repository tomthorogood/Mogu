/*
 *
 *  Created on: Aug 14, 2012
 *      Author: tom
 */

#ifndef VALIDATORPARSER_H_
#define VALIDATORPARSER_H_

#include <Wt/WValidator>
#include <Redis/MoguQueryHandler.h>

namespace Validators {

Wt::WValidator* create_validator(const std::string& validator);
Wt::WRegExpValidator* create_regex_validator(const std::string& pattern);

}    //namespace Validators

#endif /* VALIDATORPARSER_H_ */
