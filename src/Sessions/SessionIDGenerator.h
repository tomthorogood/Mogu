/*
 * SessionIDGenerator.h
 *
 *  Created on: Aug 1, 2012
 *      Author: tom
 */

#ifndef SESSIONIDGENERATOR_H_
#define SESSIONIDGENERATOR_H_

#include <declarations.h>

namespace Sessions
{
namespace Generator
{

std::string generate_id(TokenCycles* packet, std::string current_id =EMPTY);


}
}


#endif /* SESSIONIDGENERATOR_H_ */
