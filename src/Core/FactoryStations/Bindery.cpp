/*
 * Bindery.cpp
 *
 *  Created on: Sep 28, 2012
 *      Author: tom
 */

#include <Core/FactoryStations/Bindery.h>
#include <Redis/RedisCore.h>
#include <Redis/ListReturnGenerator.h>

namespace Goo{
namespace MoldableFactory{
namespace Bindery{

void bind_all_events (Moldable* widget)
{
	using namespace GoMogu;
	std::string keyspace = widget->getProperties()->node + events_keyspace;
	Redis::ListReturnGenerator gen("keys "+keyspace);
	std::string nextEvent = EMPTY;
	do
	{
		nextEvent = gen.next();
		bind_event(nextEvent,widget);
	} while (nextEvent != EMPTY);

}

void bind_event(const std::string& event, Moldable* widget)
{

}


}//namespace Bindery
}//namespace MoldableFactory
}//namespace Goo
