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

void create_auth_token(
		string sessionid,
		string userid,
		string userauth,
		TokenCycles* envelope)
{
	string ustr,hstr,estr;
	ustr = sessionid+userid+userauth;
	hstr = Hash::toHash(ustr);
	estr = encrypt(hstr);
	envelope->first = estr;
	envelope->second = 0;

	while (Sessions::Lookups::auth_token_exists(envelope->first))
	{
		envelope->first = collision_proof(envelope->first);
		envelope->second++;
	}
	mApp;
	app->redisCommand("sadd", __NODE_ALL_AUTHS, envelope->first);
	app->freeReply();
}

string generate_salt()
{
	TurnLeft::Utils::RandomCharSet r;
	return r.generate(3);
}

string encrypt(string dstr)
{
	BlowfishKeyCreator k;
	PacketCenter e(dstr, DECRYPTED);
	e.giveKey(k.getKey());
	return e.encrypt();
}

string decrypt(string estr, PacketType translation)
{
	TurnLeft::Utils::sreplace(estr, '_', ' ');
	BlowfishKeyCreator k;
	PacketCenter d(estr, ENCRYPTED);
	d.giveKey(k.getKey());
	return d.decrypt(translation);
}

}//namespace Security
