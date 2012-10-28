/*
 * Action_Store.cpp
 *
 *  Created on: Oct 23, 2012
 *      Author: tom
 */

#ifndef ACTION_STORE_CPP_
#define ACTION_STORE_CPP_

#include <Events/MoldableActionCenter.h>
#include <Redis/StorageRequest.h>
#include <Types/Listener.h>

namespace Events{
namespace ActionCenter{
namespace Actions{

bool store(Listeners& listeners, Nodes::NodeValue& message)
{
	size_t num_listeners = listeners.size();
	for (size_t w = 0; w < num_listeners; w++)
	{
		Redis::StorageRequest r(listeners.at(w)->getString(), message);
		if (!r.execute()) return false;
	}
	return true;
}

}//namespace Actions
}//namespace ActionCenter
}//namespace Events

#endif /* ACTION_STORE_CPP_ */
