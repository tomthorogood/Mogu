/*
 * UserCreator.h
 *
 *  Created on: Oct 22, 2012
 *      Author: tom
 */

#ifndef USERCREATOR_H_
#define USERCREATOR_H_
#include <declarations.h>

namespace Security{

class UserManager
{
	Mogu& application;
	std::string __plain_userlogin;
	std::vector <std::string> __sessions;
	std::string __mogu_userid;
	std::string __plain_password;
	std::string __enc_userlogin;
	std::string __salt;

	std::string __recent_session;

	inline void __encid_nempty()
	{
		if (__enc_userlogin == EMPTY) return;
		//TODO THROW ERRROR!
	}

	inline void __userid_nempty()
	{
		if (__mogu_userid == EMPTY) return;
		//TODO THROW ERROR!
	}

	void __get_recent_session__();
	inline std::string __get_auth_token()
	{
		return getAuthToken(__recent_session);
	}

	std::string __create_auth_string();
	bool __user_exists();
	void __get_salt();

	void __populate_sessions();

public:
	UserManager(Mogu& app) : application(app) {};

	/*!\brief Adds a new user for the application.
	 *
	 *
	 * @param plain_id		The user's plaintext id
	 * @param plain_passwd	The user's plaintext password
	 * @return Whether or not the creation was successful
	 *
	 * The steps for this are as follows:
	 *
	 * + Verify that the user does not already exist
	 * + Generate salt for the user
	 * + Create the user's unique auth string
	 * + Create a new session
	 * + Link the new session to the "global" session
	 * + Create a new authentication token
	 * + Link the unique auth string to the authentication token
	 * + Link the session id to the authentication token
	 *
	 */
	bool createUser(
			const std::string& plain_login, const std::string& plain_passwd);
	bool userLogin(
			const std::string& plain_login, const std::string& plain_passwd);
	bool resetPassword(const std::string& plain_id);
	bool changePassword(const std::string& new_password);
	std::string getAuthToken(const std::string& sessionid);

	inline const std::string& getMoguID()
		{ return __mogu_userid; }

	inline const std::vector <std::string>& getUserSessions()
	{
		if (__sessions.size() == 0) __populate_sessions();
		return __sessions;
	}
};

}//namespace Security



#endif /* USERCREATOR_H_ */
