/*
 * ValidatorParser.h
 *
 *  Created on: Aug 14, 2012
 *      Author: tom
 */

#ifndef VALIDATORPARSER_H_
#define VALIDATORPARSER_H_

#include <declarations.h>
namespace Validators {

Wt::WValidator* createValidator(std::string validator);
Wt::WRegExpValidator* createRegexValidator(Redis::ContextQuery&,const char*);

}    //namespace Validators

#endif /* VALIDATORPARSER_H_ */
