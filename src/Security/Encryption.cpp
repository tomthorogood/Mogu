/*
 * encryption.cpp
 *
 *  Created on: Oct 24, 2012
 *      Author: tom
 */

#include <declarations.h>
#include <crypt/BlowfishKey.h>
#include <crypt/Packet.h>

namespace Security {

std::string encrypt(std::string dstr)
{
    BlowfishKeyCreator k;
    PacketCenter e(dstr, DECRYPTED);
    e.giveKey(k.getKey());
    return e.encrypt();
}

std::string decrypt(std::string estr, PacketType translation)
{
    TurnLeft::Utils::sreplace(estr, '_', ' ');
    TurnLeft::Utils::trimchar(estr);
    BlowfishKeyCreator k;
    PacketCenter d(estr, ENCRYPTED);
    d.giveKey(k.getKey());
    return d.decrypt(translation);
}

}    //namespace Security
