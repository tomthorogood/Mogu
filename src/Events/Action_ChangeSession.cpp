/*
 * Action_ChangeSession.cpp
 *
 *  Created on: Aug 14, 2012
 *      Author: tom
 */

#include <hash.h>
#include <crypt/BlowfishKey.h>
#include <crypt/PacketCenter.h>
#include <assert.h>
#include <Static.h>
#include <Mogu.h>
#include <Exceptions/Exceptions.h>
#include <Security/Security.h>

namespace Events{
namespace ActionCenter{
namespace Actions{
using std::string;

namespace{
	const string userid_key = "USERID";
	const string userauth_key = "USERAUTH";
}

bool change_session ()
{
	mApp;
	string p_userid,p_userauth;
	p_userid = app->getSlots()[userid_key];
	p_userauth = app->getSlots()[userauth_key];
	TurnLeft::Utils::stolower(p_userid);
	return app->getManager().userLogin(p_userid, p_userauth);
}


bool register_user()
{
	mApp;
	string p_userid, p_userauth;
	p_userid = app->getSlots()[userid_key];
	p_userauth = app->getSlots()[userauth_key];
	TurnLeft::Utils::stolower(p_userid);
	return app->getUserManager().createUser(p_userid, p_userauth);
}

} //namespace Actions
} //namespace ActionCenter
} //namespace Actions


