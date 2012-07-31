/*
 * PerspectiveHandler.h
 *
 *  Created on: Jul 30, 2012
 *      Author: tom
 */

#ifndef PERSPECTIVEHANDLER_H_
#define PERSPECTIVEHANDLER_H_

#include <declarations.h>

namespace Perspective{
namespace Handler{

/* Alters the state of a widget; functions much like a Moldable Action, but
 * without signal broadcasts.
 */
void mold(std::string perspective);



} //namespace Handler
} //namespace Perspective


#endif /* PERSPECTIVEHANDLER_H_ */
