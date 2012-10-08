/*
 * PropertyCallbacks.h
 *
 *  Created on: Oct 8, 2012
 *      Author: tom
 */

#ifndef PROPERTYCALLBACKS_H_
#define PROPERTYCALLBACKS_H_

#include <declarations.h>

namespace Goo
{
namespace MoldableFactory
{
namespace Callbacks
{

std::string __value_wtext(Moldable& widget);
std::string __value_wlineedit(Moldable& widget);
std::string __value_abstract(Moldable& widget);
}

}//namespace Callbacks
}//namespace ModlableFactory
}//namespace Goo


#endif /* PROPERTYCALLBACKS_H_ */
