/*
 * Security.cpp
 *
 *  Created on: Aug 22, 2012
 *      Author: tom
 */

#include <crypt/BlowfishKey.h>
#include <crypt/PacketCenter.h>
#include <TurnLeftLib/Utils/randomcharset.h>
#include <Security/Security.h>
#include <hash.h>


namespace Security
{
using std::string;
string create_auth_string(
		string userid,
		string salt,
		string userauth)
{
	string ustr,hstr,estr;

	ustr = userid+salt+userauth;
	hstr = Hash::toHash(ustr);
	estr = encrypt(hstr);
	return estr;
}

string create_auth_token(
		string sessionid,
		string userid,
		string userauth)
{
	string ustr,hstr,estr;
	ustr = sessionid+userid+userauth;
	hstr = Hash::toHash(ustr);
	estr = encrypt(hstr);
	return estr;
}

string generate_salt()
{
	TurnLeft::Utils::RandomCharSet r;
	return r.generate(3);
}

string encrypt(string dstr)
{
	BlowfishKeyCreator k;
	PacketCenter e(dstr);
	e.giveKey(k.getKey());
	return e.encrypt();
}

string decrypt(string estr)
{
	BlowfishKeyCreator k;
	PacketCenter d(estr);
	d.giveKey(k.getKey());
	return d.decrypt();
}

}//namespace Security
