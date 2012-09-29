/*
 * Listeners.h
 *
 *  Created on: Sep 28, 2012
 *      Author: tom
 */

#ifndef LISTENERS_H_
#define LISTENERS_H_

#include <stdarg.h>
#include <Static.h>
namespace Goo
{
namespace MoldableFactory{

namespace Listeners{

typedef void (*PacketFiller)(Moldable*, std::vector <Moldable*>&,...);

inline void parent(Moldable* __child, std::vector <Moldable*>& __list,...)
{
	Moldable* __parent = (Moldable*) __child->parent();
	__list.push_back(__parent);
}

inline void siblings(Moldable* __child, std::vector <Moldable*>& __list,...)
{
	Moldable* __parent = (Moldable*) __child->parent();
	size_t all_children = __parent->countMoldableChildren();
	for (size_t c = 0; c < all_children; c++)
	{
		Moldable* child = __parent->child(c);
		if (child != __child) __list.push_back(child);
	}
}

inline void children(Moldable* __child, std::vector <Moldable*>& __list,...)
{
	size_t num_children = __child->countMoldableChildren();
	for (size_t c = 0; c < num_children; c++)
	{
		__list.push_back(__child->child(c));
	}
}

inline void registeredWidget(Moldable* __child, std::vector <Moldable*>& __list,...)
{
	va_list args;
	va_start(args,__list);
	std::string name = va_arg(args, std::string);
	__list.push_back( Application::mogu()->registeredWidget(name));
}

inline void self(Moldable* __child, std::vector <Moldable*>& __list,...)
{
	__list.push_back(__child);
}

}//namespace Listeners
}//namespace MoldableFactory
}//namespace Goo


#endif /* LISTENERS_H_ */
