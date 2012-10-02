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
		token_cycles 	= 0;
		session_cycles 	= 0; //currently unused. deprecate?
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
		using namespace Redis;
		using Sessions::Lookups::prhshd_session_node;
		string next_meta = prhshd_session_node(p->next_session, __META_HASH);;

		command("hset", next_meta, __PREV_HASH, p->last_session);
		clear();
		command("hset", next_meta, __AUTH_HASH, p->auth_token);
		clear();
		command("hset", __NODE_AUTH_LOOKUP, p->auth_string, p->auth_token);
		clear();
		if (p->token_cycles > 0)
		{
			std::stringstream s;
			s << p->token_cycles;
			command("hset",
					__NODE_COLLISION_TOK_LOOKUP, p->e_userid, s.str());
			clear();
		}
		command("hset", __NODE_SESSION_LOOKUP, p->e_userid, p->next_session);
		clear();
	}
}



bool change_session ()
{
	//___SETUP___//
	using namespace Application;
	using namespace Exceptions;
	using namespace Sessions;
	using namespace Sessions::SubmissionHandler;
	using namespace Sessions::Lookups;
	using namespace Redis;
	using namespace Hash;
	using Security::collision_proof;


	if (!__init__) init();

	const string mogu_session			= mogu()->sessionId();

	string
		p_userid
		,p_userauth
		,e_userid
		,salt
		,e_auth_string
		,usr_last_session
		,last_auth
		,access_auth
	;

	//__BODY__//
	try
	{
		p_userid = retrieveSlot(userid_key, mogu_session);
		TurnLeft::Utils::stolower(p_userid);
		p_userauth = retrieveSlot(userauth_key, mogu_session);
	}
	catch(const Err_SlotLookupFailed& e)
	{
		/* If there is no data stored in the slots when we look it up,
		 * then we must notify programmer that the slot events are not
		 * firing before calling this method.
		 */
		throw Err_EventOrderException(
				"Widget unknown.",
				"Attempted to retrieve slot data before "
				"setting it. Are you sure you've called the slot directives?");
	}

	TurnLeft::Utils::stolower(p_userid);
	e_userid 	= Security::encrypt(p_userid);

	// Make sure the user exists
	if (!hashkey_exists(__NODE_SESSION_LOOKUP, e_userid)) return false;

	// Next, get the user's salt and session, and construct their auth string.
	salt = user_salt(e_userid);
	usr_last_session = Lookups::last_session(p_userid);

	e_auth_string = Security::create_raw_auth_string(
			p_userid, salt, p_userauth);

	/* Determine if this string was collision proofed upon creation,
	 * and if so, get the number of collision proofing cycles and run them:
	 */

	std::string authstring_cycles = proofed_authstring(e_userid);
	if (authstring_cycles != EMPTY)
	{
		int num_cycles = atoi(authstring_cycles.c_str());
		for (int c = 0; c < num_cycles; c++)
		{
			e_auth_string = collision_proof(e_auth_string);
		}
	}

	/* Now that we have their auth string, we can make sure that it exists
	 * in the auth_lookup table:
	 */
	if (!hashkey_exists(__NODE_AUTH_LOOKUP, e_auth_string)) return false;

	/* Now, we'll retrieve the auth token to make sure that it matches
	 * the one stored in the user's last_session.
	 */
	std::string last_meta = prhshd_session_node(usr_last_session, __META_HASH);
	last_auth = raw_last_authtoken(usr_last_session);

	std::string authtok_cycles = proofed_last_authtoken(e_userid);
	if (authtok_cycles != EMPTY)
	{
		int num_cycles = atoi(authtok_cycles.c_str());
		for (int c = 0; c < num_cycles; c++)
		{
			last_auth = collision_proof(last_auth);
		}
	}

	access_auth = access_token(e_auth_string);

	TokenCycles session_packet;
	Sessions::Generator::generate_id(&session_packet,salt);

	TokenCycles token_packet;
	Security::create_auth_token(
			session_packet.first,
			p_userid,
			p_userauth,
			&token_packet);

	SessionParams sessionParams;
	sessionParams.next_session 	= session_packet.first;
	sessionParams.last_session 	= usr_last_session;
	sessionParams.auth_string 	= e_auth_string;
	sessionParams.auth_token 	= token_packet.first;
	sessionParams.e_userid 		= e_userid;
	sessionParams.token_cycles 	= token_packet.second;
	sessionParams.session_cycles= session_packet.second;
	add_session(&sessionParams);
	setSessionID(session_packet.first);
	setAuthToken(token_packet.first);
	return true;
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


	const string mogu_session = mogu()->sessionId();

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
	command("hset", __NODE_SALT_LOOKUP, e_userid, salt);
	clear();

	//Create the user's auth string and set up the collision table, if necessary
	auth_str_packet.first = Security::create_raw_auth_string(
			p_userid, salt, p_userauth);
	Security::proof_auth_string(&auth_str_packet);
	if (auth_str_packet.second > 0)
	{
		std::stringstream s;
		s << auth_str_packet.second;
		command("hset", __NODE_COLLISION_STR_LOOKUP,e_userid, s.str());
		clear();
	}


	Security::create_auth_token(
			session_packet.first, p_userid, p_userauth, &auth_token_packet);

	const std::string contact_storage = Hash::toHash("contact");
	const std::string email_hash = Hash::toHash("contact_email");
	std::string contact_node = prhshd_session_node(
			session_packet.first, contact_storage);
	Redis::command("hset", contact_node, email_hash, e_userid);
	Redis::clear();
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
	setSessionID(session_packet.first);
	setAuthToken(auth_token_packet.first);

	//___CLEANUP___//
	return true;
}

} //namespace Actions
} //namespace ActionCenter
} //namespace Actions


