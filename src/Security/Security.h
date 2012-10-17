/*
 * Security.h
 *
 *  Created on: Aug 22, 2012
 *      Author: tom
 */

#ifndef SECURITY_H_
#define SECURITY_H_

#include <declarations.h>
#include <crypt/PacketCenter.h>

#include <hash.h>

namespace Security{

std::string create_raw_auth_string(
		std::string userid
		,std::string salt
		,std::string userauth);

void proof_auth_string(TokenCycles* packet);

void create_auth_token(
		std::string sessionid
		,std::string userid
		,std::string userauth
		,TokenCycles* packet
);

std::string generate_salt();
std::string encrypt(std::string decrypted);
std::string decrypt(std::string encrypted, PacketType translation=DO_TRANSLATION);

inline std::string collision_proof (std::string estr)
{
	return Security::encrypt ( Hash::toHash(estr) );
}

}//namespace Security


#endif /* SECURITY_H_ */
