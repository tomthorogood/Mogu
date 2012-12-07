/*
 * LoginAuthenticator.h
 *
 *  Created on: Oct 22, 2012
 *      Author: tom
 */

#ifndef LOGINAUTHENTICATOR_H_
#define LOGINAUTHENTICATOR_H_

#include <declarations.h>

namespace Security {

/*!\brief This class is used for checking the user input information against
 * the database. Every lock that isunlocked, the corresponding bit is turned on
 * in the __locks data. The user is only logged in if all bits are on.
 *
 * The basic algorithm is as follows:
 *
 * + Make userid lowercase
 * + Encrypt userid
 * + Lookup user salt
 * + Use plain userid, salt, and password to construct raw auth string
 * + Look up authstring collisions for username, and run collision algorithm on raw string.
 * + Ensure that newfly formed proofed authstring exists
 * + Look up user's last session and extract raw auth token
 * + Determine whether last session auth token was collision proofed and
 * 	if so, run the collision algorithm.
 * + Check to see if the proofed auth token associated with the proofed auth string matches.
 *  if so, the user is authenticated.
 *
 */

struct AuthPackage
{
    std::string mogu_userid;
    std::string proofed_auth_string;
    std::string salt;
    std::string last_session;
};

class LoginAuthenticator
{
    enum lockbits
    {
        userid_exists = 0x1,
        userid_has_salt = 0x2,
        proofed_auth_string_exists = 0x4,
        proofed_auth_token_matches = 0x8,
        START_STATE = 0xF0,
        END_STATE = 0xFF
    };

    std::string plain_userlogin;	//set in constructor
    std::string enc_userlogin;		//set in constructor
    std::string mogu_userid;		//set in user_exists()
    std::string plain_passwd; 		// set in constructor
    std::string salt;				// set in user_salted()
    std::string raw_auth_string;	// set in authstring_exists()
    std::string prf_auth_string;	// set in authstring_exists()
    std::string last_auth_token;	// set in authtoken_matches()
    std::string prf_auth_token;		// set in authtoken_matches()
    std::string last_session_id;	// set in user_exists()

    uint8_t __locks;				// defaults to 1111 0000

    bool user_exists();
    bool user_salted();
    bool authstring_exists();
    bool authtoken_matches();
    void proof_token(
        std::string& token, std::string proof_table, const std::string& key);
public:
    LoginAuthenticator(
        const std::string& username, const std::string& passwd);
    inline bool authenticated()
    {
        return __locks == END_STATE;
    }
    inline void fillAuthPackage(
        AuthPackage& pkg)
    {
        if (!authenticated()) return;
        pkg.mogu_userid = mogu_userid;
        pkg.last_session = last_session_id;
        pkg.salt = salt;
        pkg.proofed_auth_string = prf_auth_string;
    }
};

}				//namespace Security

#endif /* LOGINAUTHENTICATOR_H_ */
