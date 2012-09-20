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

void generate_id(TokenCycles* packet, std::string salt =EMPTY);


}
}


#endif /* SESSIONIDGENERATOR_H_ */
