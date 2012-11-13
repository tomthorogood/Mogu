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

string generate_salt()
{
	TurnLeft::Utils::RandomCharSet r;
	return r.generate(3);
}



}//namespace Security
