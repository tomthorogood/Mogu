/*
 * Security.h
 *
 *  Created on: Aug 22, 2012
 *      Author: tom
 */

#ifndef SECURITY_H_
#define SECURITY_H_

#include <declarations.h>
#include <Security/Encryption.h>
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
		,std::string p_userid
		,std::string p_userauth
		,TokenCycles* packet
);

std::string generate_salt();


inline std::string collision_proof (std::string estr)
{
	return encrypt ( Hash::toHash(estr) );
}

}//namespace Security


#endif /* SECURITY_H_ */
