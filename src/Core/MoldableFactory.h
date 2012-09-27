/*
 * MoldableFactory.h
 *
 *  Created on: Sep 27, 2012
 *      Author: tom
 */

#ifndef MOLDABLEFACTORY_H_
#define MOLDABLEFACTORY_H_

#include <declarations.h>

namespace Goo{

namespace MoldableFactory{

MoldableTemplate* conceptualize(const std::string& nodeName);
MoldableTemplate* conceptualize(const MoldableTemplate* t);
Moldable* sculpt (MoldableTemplate*);

}

}//namespace Goo


#endif /* MOLDABLEFACTORY_H_ */
