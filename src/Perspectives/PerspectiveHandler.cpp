/*
 * PerspectiveHandler.cpp
 *
 *  Created on: Jul 30, 2012
 *      Author: tom
 */

#include <Perspectives/PerspectiveHandler.h>
#include <Redis/RedisCore.h>
#include <Events/EventPreprocessor.h>
#include <Events/BroadcastMessage.h>
#include <Events/MoldableActionCenter.h>
#include <Mogu.h>

namespace Perspective{
namespace Handler{


void mold(std::string perspective)
{
	namespace Action = Enums::SignalActions;
	size_t num_molds = 0;
	Redis::strvector keyspace;

	mApp;
	std::string spersp = "perspectives."+perspective+".*";
	const char* cpersp = spersp.c_str();
	app->redisCommand("keys %s",cpersp);
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
