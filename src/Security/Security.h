/*
 * Security.h
 *
 *  Created on: Aug 22, 2012
 *      Author: tom
 */

#ifndef SECURITY_H_
#define SECURITY_H_

#include <declarations.h>

namespace Security{

std::string create_auth_string(
		std::string userid,
		std::string salt,
		std::string userauth);

std::string create_auth_token(
		std::string sessionid,
		std::string userid,
		std::string userauth);

std::string generate_salt();
std::string encrypt(std::string decrypted);
std::string decrypt(std::string encrypted);

}


#endif /* SECURITY_H_ */
