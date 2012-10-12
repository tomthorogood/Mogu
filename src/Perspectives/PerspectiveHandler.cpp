/*
 * PerspectiveHandler.cpp
 *
 *  Created on: Jul 30, 2012
 *      Author: tom
 */

#include <Perspectives/PerspectiveHandler.h>
#include <Redis/RedisCore.h>
#include <TurnLeftLib/Utils/HungryVector.h>
#include <Static.h>
#include <Mogu.h>
#include <Core/Moldable.h>
#include <Parsers/NodeValueParser.h>
#include <Parsers/Parsers.h>
#include <Types/NodeValue.h>
#include <Wt/WStackedWidget>
#include <Events/MoldableActionCenter.h>
#include <Events/EventPreprocessor.h>

namespace Perspective{
namespace Handler{


void mold(std::string perspective)
{
	namespace Action = Enums::SignalActions;
	std::string action_str;
	std::string message_str;
	size_t num_molds = 0;
	Redis::strvector keyspace;

	mApp;
	app->redisCommand("keys","perspectives."+perspective+".*");
	Redis::toVector(app->reply(),keyspace);
	num_molds = keyspace.size();

	for (size_t w = 0; w < num_molds; w++)
	{
		Events::EventPreprocessor preproc(keyspace[w]);
		Events::BroadcastMessage msg(NULL, &preproc);
		Events::ActionCenter::submitBroadcast(msg);
	}
}

} //namespace Perspective
} //namespace Handler
