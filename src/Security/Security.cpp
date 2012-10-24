/*
 * Security.cpp
 *
 *  Created on: Aug 22, 2012
 *      Author: tom
 */

#include <crypt/BlowfishKey.h>
#include <TurnLeftLib/Utils/randomcharset.h>
#include <TurnLeftLib/Utils/inlines.h>
#include <Security/Security.h>
#include <Sessions/Lookups.h>
#include <hash.h>


namespace Security
{
using std::string;

std::string create_raw_auth_string(
		string userid
		,string salt
		,string userauth)
{
	string ustr = userid+salt+userauth;
	string hstr = Hash::toHash(ustr);
	return encrypt(hstr);
}

void proof_auth_string(TokenCycles* packet)
{
	while (Sessions::Lookups::auth_string_exists(packet->first))
	{
		packet->first = collision_proof(packet->first);
		packet->second++;
	}
}

string generate_salt()
{
	TurnLeft::Utils::RandomCharSet r;
	return r.generate(3);
}



}//namespace Security
