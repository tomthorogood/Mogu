/*
 * UserManager.cpp
 *
 *  Created on: Oct 22, 2012
 *      Author: tom
 */


#include <Security/UserManager.h>
#include <Mogu.h>
#include <TurnLeftLib/Utils/inlines.h>
#include <TurnLeftLib/Utils/randomcharset.h>
#include <Exceptions/Exceptions.h>
#include <Security/LoginAuthenticator.h>
#include <Security/Security.h>
#include <Redis/RedisUtils.h>
#include <Types/EmailManager.h>

namespace Security{

std::string UserManager::__create_auth_string()
{
	UniqueHashPackage pkg(
			create_raw_auth_string(
					__plain_userlogin, __salt, __plain_password)
			,__NODE_AUTH_STR_SET);
	Redis::makeUniqueHash(pkg);
	pkg.lock();
	if (pkg.num_cycles > 0)
	{
		application.redisExec(Mogu::Discard, "hset %s %s %d"
				, __NODE_COLLISION_STR_LOOKUP
				, cuserid
				, pkg.num_cycles);
	}
	return pkg.proofed_hash;
}

bool UserManager::createUser(
		const std::string& plain_id, const std::string& plain_passwd)
{
	__plain_userlogin = plain_id;
	__plain_password = plain_passwd;

	TurnLeft::Utils::stolower(__plain_userlogin);
	__enc_userlogin = Security::encrypt(__plain_userlogin);
	const char* cenc_id = __enc_userlogin.c_str();

	// Ensure that the user does not already exist
	if (__user_exists()) return false;

	// Generate a userid
	__mogu_userid = Redis::makeUniqueRChar(__NODE_USERID_SET);
	const char* cuserid = __mogu_userid.c_str();
	application.redisExec(Mogu::Discard, "sadd %s %s",
			__NODE_USERID_SET, cuserid);

	// Assign the userid to the encrypted login:
	application.redisExec(Mogu::Discard, "hset %s %s %s",
			__NODE_USERID_LOOKUP, cenc_id, cuserid);


	// Generate salt and add it to the database
	__salt = generate_salt();
	const char* csalt = __salt.c_str();
	application.redisExec(Mogu::Discard, "hset %s %s %s"
			, __NODE_SALT_LOOKUP, cuserid, csalt);

	std::string auth_string = __create_auth_string();
	const char* cprhash_auth = auth_string.c_str();



	// Create an auth token for the session
	std::string auth_token = Redis::makeUniqueRChar(__NODE_AUTH_TOK_SET);
	const char* cauth_token = auth_token.c_str();
	application.redisExec(Mogu::Discard, "sadd %s %s",
			__NODE_AUTH_TOK_SET, cauth_token);

	// Associate the auth string and the auth token:
	application.redisExec(Mogu::Discard,"hset %s %s %s"
			, __NODE_AUTH_LOOKUP
			, cprhash_auth
			, cauth_token);

	Security::AuthPackage authPkg;
	authPkg.mogu_userid = __mogu_userid;
	authPkg.last_session = "global";
	authPkg.proofed_auth_string = auth_string;
	authPkg.salt = __salt;
	application.getManager().createNewSession(authPkg);
	return true;
}

bool UserManager::userLogin(
		const std::string& plain_userID, const std::string& userauth)
{
	mApp;
	LoginAuthenticator auth(plain_userID, userauth);
	if (!auth.authenticated()) return false;
	AuthPackage authPkg;
	//Avoids having to re-retrieve data that we'll need again.
	auth.fillAuthPackage(authPkg);
	app->getManager().createNewSession(authPkg);
	return true;
}

void UserManager::__get_recent_session__()
{
	__userid_nempty();
	application.redisExec(Mogu::Keep, "hget %s %s",
			__NODE_SESSION_LOOKUP, __mogu_userid.c_str());
	__recent_session = Redis::toString(application.reply());
}

bool UserManager::__user_exists()
{
	__encid_nempty();
	application.redisExec(Mogu::Keep, "hexists %s %s",
			__NODE_USERID_LOOKUP,__enc_userlogin.c_str());
	return (bool) Redis::getInt(application.reply());
}

void UserManager::__get_salt()
{
	const char* cuserid = __mogu_userid.c_str();
	application.redisExec(Mogu::Keep, "hexists %s %s"
			,__NODE_SALT_LOOKUP, cuserid);

	if (!(bool)Redis::getInt(application.reply()))
		throw Exceptions::Err_NonExistentField(
				"s.global.&salt_lookup&", cuserid, "__get_salt()");

	application.redisExec(Mogu::Keep, "hget %s %s",
			__NODE_SALT_LOOKUP, cuserid);
	__salt = Redis::toString(application.reply());
}

std::string UserManager::getAuthToken(const std::string& sessionid)
{
	std::string session_node = "s."+sessionid+"."+__META_HASH;
	application.redisExec(Mogu::Keep, "hget %s %s"
			, session_node.c_str(), __AUTH_HASH);
	return Redis::toString(application.reply());
}

bool UserManager::resetPassword(const std::string& plain_userid)
{
	TurnLeft::Utils::RandomCharSet r;
	const std::string EMAIL_LOOKUP = "[contact] &email&";
	std::string plain_newpass = r.generate(4);
	__plain_userlogin = plain_userid;
	TurnLeft::Utils::stolower(__plain_userlogin);
	__enc_userlogin = Security::encrypt(plain_userid);

	application.redisExec(Mogu::Keep, "hexists %s %s"
			,__NODE_USERID_LOOKUP, __enc_userlogin.c_str()
	);
	if (! (bool) Redis::getInt(application.reply())) return false;

	application.redisExec(Mogu::Keep, "hget %s %s"
			,__NODE_USERID_LOOKUP, __enc_userlogin.c_str());
	__mogu_userid = Redis::toString(application.reply());
	__get_recent_session__();

	application.setSessionID(__recent_session);
	NodeValue val;
	application.interpreter().giveInput(EMAIL_LOOKUP, val);
	TurnLeft::Utils::trimchar(val.getString());
	EmailManager email;
	email.setRecipient(val.getString());
	email.setSubject("Your FinancialFirsts.org Password Request");
	email.setMessage(
		"You recently requested that FinancialFirsts.org reset your password."
		"Your new password is below, and you may start using it immediately."
		"It is highly recommended that you change your password when you "
		"next log in, and delete this email afterward."
		"Your new password is: " + plain_newpass +"\n\n"
		"Thanks,\n The Financial Firsts Team"
	);

	bool success = changePassword(plain_newpass);

	if (success) email.send();
	return success;
}

bool UserManager::changePassword(const std::string& plain_newpass)
{
	__plain_password = plain_newpass;
	if (!__user_exists()) return false;
	__get_salt();
	__get_recent_session__();
	std::string recent_auth_token = __get_auth_token();
	const char* old_auth_string = Redis::reverseHashLookup(
			recent_auth_token, __NODE_AUTH_LOOKUP)
			.c_str();
	std::string new_auth_string = __create_auth_string();

	application.redisExec(Mogu::Discard, "hset %s %s %s"
			, __NODE_AUTH_LOOKUP,
			new_auth_string.c_str(),
			recent_auth_token.c_str());

	application.redisExec(Mogu::Discard, "hdel %s %s"
			, __NODE_AUTH_LOOKUP, old_auth_string);
	application.redisExec(Mogu::Discard, "srem %s %s"
			, __NODE_AUTH_STR_SET, old_auth_string);

	return true;
}

}//namespace Security


