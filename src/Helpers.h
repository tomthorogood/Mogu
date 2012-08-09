/*
 * Helpers.h
 *
 *  Created on: Aug 9, 2012
 *      Author: tom
 */

#ifndef HELPERS_H_
#define HELPERS_H_

#include <declarations.h>

namespace Helpers{
inline std::string getTemplateNode (Goo::Moldable* widget)
{
	return widget->getNodeList()->at(0);
}

}//namespace Helpers;
#endif /* HELPERS_H_ */
