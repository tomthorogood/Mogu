/*
 * encryption.cpp
 *
 *  Created on: Oct 24, 2012
 *      Author: tom
 */

#include <Security/Security.h>
#include <crypt/BlowfishKey.h>
#include <crypt/Packet.h>

namespace Security{

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
