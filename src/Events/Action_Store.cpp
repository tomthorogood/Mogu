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
#include <Parsers/MoguScript_Tokenizer.h>
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
		Nodes::NodeValue v;
		Parsers::MoguScript_Tokenizer t(listeners.at(w)->getString());
		std::string policy_name = t.next();
		Parsers::NodeValueParser p(policy_name, v);

		std::string policy = v.getString();

		std::string arg = EMPTY;


		Redis::RedisStorageRequest r(
				policy
				,app->sessionID()
				,message);


		if (!r.execute()) return false;
	}
	return true;
}

}//namespace Actions
}//namespace ActionCenter
}//namespace Events

#endif /* ACTION_STORE_CPP_ */
