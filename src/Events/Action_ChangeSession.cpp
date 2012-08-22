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
	string prev_session;
	string auth_field;
	string meta_hash;
	string session_lookup;
	string auth_lookup;
	string salt_lookup;
	bool __init__ = false;

	void init ()
	{
		using namespace Hash;
		prev_session = toHash("prev");
		auth_field = toHash("auth");
		meta_hash = toHash("meta");
		salt_lookup = toHash("salt");
		session_lookup = toHash(SESSION_LOOKUP);
		auth_lookup = toHash(AUTH_LOOKUP);
		__init__ = true;
	}
}

bool change_session ()
{
	//___SETUP___//
	using namespace Application;
	using namespace Exceptions;
	using namespace Sessions;
	using namespace Redis;
	using namespace Hash;


	if (!__init__) init();

	string mogu_session			= mogu()->sessionId();
	BlowfishKeyCreator* keygen	= new BlowfishKeyCreator();
	BF_KEY* enc_key				= keygen->getKey();

	string
		p_userid		//plaintext user id (out of scope after this method)
		,e_userid		//encrypted user id
		,salt			//user's salt
		,last_session	//id of the user's last session
		,e_auth_string	//user's encrypted auth string
		,last_auth		//auth token stored with the last session
		,next_auth		//auth token stored with the new session
		,access_auth	//auth token to test against `last_auth`
		,p_userauth		//the encrypted user password
		,next_session	//the new user session created upon successful login
	;

	//__BODY__//
	bool
		user_exists,
		password_exists,
		auth_matches;

	try
	{
		p_userid = retrieveSlot(userid_key, mogu_session);
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

	e_userid 	= Security::encrypt( Hash::toHash(p_userid) );

	// Make sure the user exists
	command("hexists s.global."+session_lookup, e_userid);
	if (! (bool) getInt())
	{
		return false;
	}

	// Next, get the user's salt and session, and construct their auth string.
	command("hget s.global."+salt_lookup, Security::encrypt(p_userid));
	salt = toString();
	command("hget s.global."+session_lookup, e_userid);
	last_session = toString();
	e_auth_string = Security::create_auth_string(p_userid, salt, p_userauth);

	/* Now that we have their auth string, we can make sure that it exists
	 * in the auth_lookup table:
	 */
	command("hexists s.global."+auth_lookup, e_auth_string);
	if (! (bool) getInt())
	{
		return false;
	}

	/* Now, we'll retrieve the auth token to make sure that it matches
	 * the one stored in the user's last_session.
	 */
	command("hget s."+last_session+"."+meta_hash, auth_field);
	last_auth = toString();
	command("hget s.global."+auth_lookup, e_auth_string);
	access_auth = toString();
	if (last_auth != access_auth) return false;

	/* Now we need to generate the new session data. */
	//TODO PREVENT HASH COLLISION
	next_session = Sessions::Generator::generate_id(salt);
	next_auth = Security::create_auth_token(
			next_session,
			p_userid,
			p_userauth);

	//TODO PREVENT HASH COLLISION
	command("hset s.global."+auth_lookup, e_auth_string, next_auth);
	clear();
	command("hset s."+next_session+"."+meta_hash, prev_session,last_session);
	clear();
	command("hset s."+next_session+"."+meta_hash, auth_field, next_auth);
	return true;
}
//TODO YOU ARE HERE
bool register_user()
{
	using namespace Application;
	using namespace Sessions;
	using namespace Redis;
	using namespace Hash;

	if (!__init__) init();

	string
		p_userid,
		p_userauth,
		h_userid,
		h_userauth,
		e_userid,
		e_userauth,
		session_name,
		p_auth_token,
		h_auth_token,
		e_auth_token;

	const string mogu_session = mogu()->sessionId();
	BlowfishKeyCreator* keygen = new BlowfishKeyCreator();
	BF_KEY* enc_key = keygen->getKey();

	p_userid 	= retrieveSlot(userid_key, mogu_session);
	p_userauth 	= retrieveSlot(userauth_key, mogu_session);
	h_userid 	= toHash(p_userid);
	h_userauth 	= toHash(p_userauth);

	PacketCenter euid(h_userid, DECRYPTED);
	PacketCenter euth(h_userauth, DECRYPTED);
	euid.giveKey(enc_key);
	euth.giveKey(enc_key);
	e_userid 	= euid.encrypt();
	e_userauth 	= euth.encrypt();

	// Make sure the username doesn't already exist //
	string node = "s.global."+session_lookup;
	command("hexists", node, e_userauth);
	if ( (bool) getInt() ) return false;

	// Otherwise, we're golden //
	session_name = Generator::generate_id(e_userid);
	p_auth_token = session_name+p_userid+p_userauth;
	h_auth_token = toHash(p_auth_token);
	PacketCenter enth(h_auth_token, DECRYPTED);
	enth.giveKey(enc_key);
	e_auth_token = enth.encrypt();

	// We now have all the encrypted data. Let's create the requisite nodes //
	command("hset", node, e_userid, session_name);
	clear();
	node = "s.global."+auth_lookup;
	command("hset", node, e_userauth, e_auth_token);
	clear();
	node = "s."+session_name+"."+meta_hash;
	command("hset", node, auth_field, e_auth_token);
	clear();
	command("hset", node, prev_session, "global");
	clear();

	// Finally, change the environment //
	setSessionID(session_name);
	setAuthToken(e_auth_token);

	//___CLEANUP___//
	delete keygen; //also deletes enc_key
	return true;
}

} //namespace Actions
} //namespace ActionCenter
} //namespace Actions


