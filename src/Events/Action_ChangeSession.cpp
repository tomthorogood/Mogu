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
#include <Sessions/Submission.h>
#include <Sessions/SessionIDGenerator.h>
#include <Sessions/Lookups.h>
#include <Mogu.h>
#include <Exceptions/Exceptions.h>
#include <Security/Security.h>

namespace Events{
namespace ActionCenter{
namespace Actions{
using std::string;

struct SessionParams
{
	std::string next_session;
	std::string last_session;
	std::string auth_string;
	std::string auth_token;
	std::string e_userid;
	int token_cycles;
	int session_cycles;

	SessionParams()
	{
		next_session 	= EMPTY;
		last_session 	= EMPTY;
		auth_string 	= EMPTY;
		auth_token 		= EMPTY;
		e_userid 		= EMPTY;
		session_cycles	= 0;
		token_cycles 	= 0;
	}
};

namespace{

	const string userid_key = "USERID";
	const string userauth_key = "USERAUTH";
	string collision_hash

	;

	bool __init__ = false;

	void init ()
	{
		using namespace Hash;
		collision_hash = toHash("collisions");
		__init__ = true;
	}

	inline void add_session(SessionParams* p)
	{
		mApp;
		using namespace Redis;
		using Sessions::Lookups::prhshd_session_node;
		string next_meta = prhshd_session_node(p->next_session, __META_HASH);;

		app->redisCommand("hset", next_meta, __PREV_HASH, p->last_session);
		app->freeReply();

		app->redisCommand("hset", next_meta, __AUTH_HASH, p->auth_token);
		app->freeReply();

		app->redisCommand("hset", __NODE_AUTH_LOOKUP, p->auth_string, p->auth_token);
		app->freeReply();

		if (p->token_cycles > 0)
		{
			std::string s = itoa(p->token_cycles);

			app->redisCommand("hset",
					__NODE_COLLISION_TOK_LOOKUP, p->e_userid, s);
			app->freeReply();

		}
		app->redisCommand("hset", __NODE_SESSION_LOOKUP, p->e_userid, p->next_session);
		app->freeReply();

	}
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
	using namespace Application;
	using namespace Exceptions;
	using namespace Sessions;
	using namespace Sessions::Lookups;
	using namespace Redis;
	using namespace Hash;

	if (!__init__) init();

	string
		p_userid
		,p_userauth
		,e_userid
		,salt
	;
	TokenCycles session_packet;
	TokenCycles auth_str_packet;
	TokenCycles auth_token_packet;

	mApp;
	const string mogu_session = app->sessionId();

	try
	{
		p_userid 	= retrieveSlot(userid_key, mogu_session);
		TurnLeft::Utils::stolower(p_userid);
		p_userauth 	= retrieveSlot(userauth_key, mogu_session);
	}
	catch (const Err_SlotLookupFailed& e)
	{
		throw Err_EventOrderException(
			"Widget unknown.",
			"Attempted to retrieve slot data before "
			"setting it. Are you sure you've called the slot directives?");
	}

	TurnLeft::Utils::stolower(p_userid);
	e_userid	= Security::encrypt(p_userid);

	// Make sure the username doesn't already exist //
	if (hashkey_exists(__NODE_SESSION_LOOKUP,e_userid))return false;
	if (hashkey_exists(__NODE_SALT_LOOKUP, e_userid)) return false;


	// Otherwise, we're golden //
	Generator::generate_id(&session_packet, e_userid);
	salt = Security::generate_salt();

	//Give the user some salt:
	app->redisCommand("hset", __NODE_SALT_LOOKUP, e_userid, salt);
	app->freeReply();

	//Create the user's auth string and set up the collision table, if necessary
	auth_str_packet.first = Security::create_raw_auth_string(
			p_userid, salt, p_userauth);
	Security::proof_auth_string(&auth_str_packet);
	if (auth_str_packet.second > 0)
	{
		std::stringstream s;
		s << auth_str_packet.second;
		app->redisCommand("hset", __NODE_COLLISION_STR_LOOKUP,e_userid, s.str());
		app->freeReply();
	}


	Security::create_auth_token(
			session_packet.first, p_userid, p_userauth, &auth_token_packet);

	const std::string contact_storage = Hash::toHash("contact");
	const std::string email_hash = Hash::toHash("contact_email");
	std::string contact_node = prhshd_session_node(
			session_packet.first, contact_storage);
	app->redisCommand("hset", contact_node, email_hash, e_userid);
	app->freeReply();
	SessionParams prms;
	prms.auth_string = auth_str_packet.first;
	prms.auth_token = auth_token_packet.first;
	prms.e_userid = e_userid;
	prms.last_session = "global";
	prms.next_session = session_packet.first;
	prms.session_cycles = session_packet.second;
	prms.token_cycles = auth_token_packet.second;
	add_session(&prms);

	// Finally, change the environment //
	app->setSessionID(session_packet.first);
	app->setAuthToken(auth_token_packet.first);

	//___CLEANUP___//
	return true;
}

} //namespace Actions
} //namespace ActionCenter
} //namespace Actions


