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

namespace Perspective {
namespace Handler {

void mold(
    std::string perspective)
{
    mApp;
    std::string spersp = "perspectives." + perspective;
    app->redisExec(Mogu::Keep, "get %s", spersp.c_str());
    int num_molds = atoi(redisReply_STRING.c_str());

    for (int w = 0; w < num_molds; w++) {
        Events::EventPreprocessor preproc(spersp, w);
        Events::BroadcastMessage msg(NULL, &preproc);
        Events::ActionCenter::submitBroadcast(msg);
    }
}

}    //namespace Perspective
}    //namespace Handler
