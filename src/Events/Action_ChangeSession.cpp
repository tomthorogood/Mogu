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
	bool __init__ = false;

	void init ()
	{
		using namespace Hash;
		prev_session = toHash("prev");
		auth_field = toHash("auth");
		meta_hash = toHash("meta");
		session_lookup = toHash(SESSION_LOOKUP);
		auth_lookup = toHash(AUTH_LOOKUP);
		__init__ = true;
	}
}

void change_session ()
{
	//___SETUP___//
	using namespace Application;
	using namespace Sessions;
	using namespace Redis;
	using namespace Hash;


	if (!__init__) init();

	string mogu_session			= mogu()->sessionId();
	BlowfishKeyCreator* keygen	= new BlowfishKeyCreator();
	BF_KEY* enc_key				= keygen->getKey();

	string
		p_userid		,//plaintext user data (out of scope after this method)
		p_userauth		,
		h_userid		,//hashed user data
		h_userauth		,
		e_userid		,//encrypted,hashed user data
		e_userauth		,
		last_session	,//name of the user's last session
		last_auth		,//the user's last auth token
		new_session		,//name of the user's newly generated session
		p_new_auth		,//plaintext new auth token
		h_new_auth		,//hashed new auth token
		e_new_auth		;//the user's new encrypted auth token

	//__BODY__//

	p_userid = retrieveSlot(userid_key, mogu_session);
	p_userauth = retrieveSlot(userauth_key, mogu_session);

#ifdef DEBUG
	assert(p_userid.substr(0,4)!="ERR_");
	assert(p_userauth.substr(0,4)!="ERR_");
#endif
	if (p_userid.substr(0,4) == "ERR_" || p_userauth.substr(0,4) == "ERR_")
	{
		//TODO THROW ERROR!
	}

		//-> Prepare for authenticity tests
	h_userid 	= toHash(p_userid);
	h_userauth 	= toHash(p_userauth);

	PacketCenter euid (h_userid, DECRYPTED);
	PacketCenter euth (h_userauth, DECRYPTED);
	euid.giveKey(enc_key);
	euth.giveKey(enc_key);
	e_userid 	= euid.encrypt();
	e_userauth 	= euth.encrypt();

	command("hget s.global."+session_lookup, e_userid);
	last_session = toString();
	command("hget s."+last_session+"."+meta_hash, "auth");
	last_auth = toString();
	command("hget s.global."+auth_lookup, e_userauth);
		//-> Test login authenticity
	if (toString() != last_auth) return; //TODO throw error instead

		//-> Generate new session data
	new_session = Generator::generate_id(last_auth);
	p_new_auth 	= new_session + p_userid + p_userauth;
	h_new_auth 	= toHash(p_new_auth);
	PacketCenter enth (h_new_auth, DECRYPTED);
	enth.giveKey(enc_key);
	e_new_auth 	= enth.decrypt();

		//-> Create new session in database
	std::string new_session_node = "s."+new_session;
	command("hset", new_session_node+"."+meta_hash, auth_field, e_new_auth);
	clear();
	command("hset", new_session_node+"."+meta_hash, prev_session, last_session);
	clear();

		//-> Change application session data
	setSessionID(new_session);
	setAuthToken(e_new_auth);

	//___CLEANUP___//
	delete keygen; //also deletes enc_key
}

void register_user()
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
	if ( (bool) getInt() ) return; //TODO THROW ERROR INSTEAD

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
}

} //namespace Actions
} //namespace ActionCenter
} //namespace Actions


