/*
 * Action_Store.cpp
 *
 *  Created on: Oct 23, 2012
 *      Author: tom
 */

#ifndef ACTION_STORE_CPP_
#define ACTION_STORE_CPP_

#include <Events/MoldableActionCenter.h>
#include <Types/RedisStorageRequest.h>
#include <Sessions/SessionHandler.h>
#include <Parsers/TokenGenerator.h>
#include <Parsers/NodeValueParser.h>
#include <Types/Listener.h>
#include <Mogu.h>

namespace Events{
namespace ActionCenter{
namespace Actions{

bool store(Listeners& listeners, Nodes::NodeValue& message)
{
	mApp;
	size_t num_listeners = listeners.size();
	for (size_t w = 0; w < num_listeners; w++)
	{
		Parsers::TokenGenerator t(listeners.at(w)->getString());
		std::string policy = t.next();
		std::string args = EMPTY;
		std::string nxt;
		do {
			nxt = t.next();
			args += nxt;
		} while (nxt != EMPTY);
		Nodes::NodeValue v;
		Parsers::NodeValueParser(args,v);
		Redis::RedisStorageRequest r(
				policy
				,app->sessionID()
				,v);
		r.setField(args);
		if (!r.execute()) return false;
	}
	return true;
}

}//namespace Actions
}//namespace ActionCenter
}//namespace Events

#endif /* ACTION_STORE_CPP_ */
