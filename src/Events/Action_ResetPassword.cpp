/*
 * Action_ResetPassword.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: tom
 */

#include <declarations.h>
#include <Security/Security.h>
#include <TurnLeftLib/Utils/randomcharset.h>
#include <TurnLeftLib/Utils/inlines.h>
#include <Redis/RedisCore.h>
#include <algorithm>
#include <Static.h>
#include <Mogu.h>
#include <Events/MoldableActionCenter.h>

namespace Events{
namespace ActionCenter{
namespace Actions{

bool change_password(std::string username, std::string new_password)
{
	return true;
}

bool reset_password(std::string username) { return true;}


}//namespace Actions
}//namespace ActionCenter
}//namespace Events
