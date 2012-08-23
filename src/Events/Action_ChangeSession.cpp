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
	string
		prev_session
		,auth_field
		,meta_hash
		,collision_hash
		,session_lookup
		,auth_lookup
		,salt_lookup
	;

	bool __init__ = false;

	void init ()
	{
		using namespace Hash;
		prev_session = toHash("prev");
		auth_field = toHash("auth");
		meta_hash = toHash("meta");
		collision_hash = toHash("collisions");
		salt_lookup = toHash("salt");
		session_lookup = toHash(SESSION_LOOKUP);
		auth_lookup = toHash(AUTH_LOOKUP);
		__init__ = true;
	}

	inline void add_session(
			string next_session
			,string last_session
			,string auth_string
			,string auth_token
			,string e_userid)
	{
		using namespace Redis;

		string next_meta = "s."+next_session+"."+meta_hash;
		string glob_auth = "s.global."+auth_lookup;
		string glob_user = "s.global."+session_lookup;

		command("hset", next_meta, prev_session, last_session);
		command("hset", next_meta, auth_field, auth_token);
		command("hset", glob_auth, auth_string, auth_token);
		command("hset", glob_user, e_userid, next_session);
	}

	inline string collision_proof (string estr)
	{
		return Security::encrypt ( Hash::toHash(estr) );
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

	const string mogu_session			= mogu()->sessionId();

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

	e_userid 	= Security::encrypt(p_userid);

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
	/* Next, determine if this string was collision proofed upon creation,
	 * and if so, get the number of collision proofing cycles and run them:
	 */
	command("hexists s.global."+auth_lookup+"."+collision_hash, e_userid);
	if ( (bool) getInt())
	{
		command("hget s.global."+auth_lookup+"."+collision_hash, e_userid);
		int num_cycles = getInt();
		for (int i = 0; i < num_cycles; i++)
		{
			e_auth_string = collision_proof(e_auth_string);
		}
	}

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

	next_session = Sessions::Generator::generate_id(salt);
	next_auth = Security::create_auth_token(
			next_session,
			p_userid,
			p_userauth);

	add_session(
			next_session, last_session, e_auth_string, next_auth, e_userid);
	setSessionID(next_session);
	setAuthToken(next_auth);
	return true;
}



//TODO YOU ARE HERE
bool register_user()
{
	using namespace Application;
	using namespace Exceptions;
	using namespace Sessions;
	using namespace Redis;
	using namespace Hash;

	if (!__init__) init();

	string
		p_userid
		,p_userauth
		,e_userid
		,e_userauth
		,e_auth_string
		,e_auth_token
		,session_name
		,salt
	;

	const string mogu_session = mogu()->sessionId();

	// (h|e) userid : sessionid
	string session_lookup_node = "s.global."+session_lookup;

	// (e) auth string : auth token
	string auth_table = "s.global."+auth_lookup;

	// (e) userid : salt
	string salt_table = "s.global."+salt_lookup;

	// (e) userid : #collisions (if not 0)
	string auth_collisions = auth_table + "." + collision_hash;

	try
	{
		p_userid 	= retrieveSlot(userid_key, mogu_session);
		p_userauth 	= retrieveSlot(userauth_key, mogu_session);
	}
	catch (const Err_SlotLookupFailed& e)
	{
		throw Err_EventOrderException(
			"Widget unknown.",
			"Attempted to retrieve slot data before "
			"setting it. Are you sure you've called the slot directives?");
	}

	e_userid	= Security::encrypt(p_userid);

	// Make sure the username doesn't already exist //
	command("hexists", session_lookup_node, e_userid);
	if ( (bool) getInt() ) return false;
	command("hexists", salt_table, e_userid); //in case of compromise
	if ( (bool) getInt() ) return false;

	// Otherwise, we're golden //
	session_name = Generator::generate_id(e_userid);
	salt = Security::generate_salt();

	//Give the user some salt:
	command("hset", salt_table, e_userid, salt);
	clear();

	e_auth_string = Security::create_auth_string(
			p_userid, salt, p_userauth);

	/* If the auth string exists in the lookup table, we must continue
	 * hashing our encrypted string and checking it against the table.
	 */
	command("hexists", auth_table, e_auth_string);
	bool auth_exists = (bool) getInt();
	int num_cycles =0;
	while (auth_exists)
	{
		/* Hash the encryption, and encrypt it again: */
		e_auth_string = collision_proof(e_auth_string);
		command("hexists", auth_table, e_auth_string);
		/* Test whether the newly generated key exists */
		auth_exists = (bool) getInt();

		/* Increase the number of collision proofing cycles. */
		++num_cycles;

		/* Convert the number of cycles to a string */
		std::stringstream st;
		st << num_cycles;

		/* Set the auth_collisions table to (e) userid : num_collisions
		 * So when they try to login again, we'll be able to perform the
		 * correct algorithms on their login credentials.
		 */
		command("hset", auth_collisions, e_userid, st.str());
		clear();

	}

	/* It really doesn't matter if this happens to be the same as another
	 * auth token, as it's only accessible through the unique auth string.
	 */
	e_auth_token = Security::create_auth_token(
			session_name, p_userid, p_userauth);

	add_session(
			session_name, "global", e_auth_string, e_auth_token, e_userid);

	// Finally, change the environment //
	setSessionID(session_name);
	setAuthToken(e_auth_token);

	//___CLEANUP___//
	return true;
}

} //namespace Actions
} //namespace ActionCenter
} //namespace Actions


