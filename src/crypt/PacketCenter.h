/*
 * PacketCenter.h
 *
 *  Created on: Jun 21, 2012
 *      Author: tom
 */

#ifndef PACKETCENTER_H_
#define PACKETCENTER_H_

#include <crypt/Packet.h>
#include <TurnLeftLib/TurnLeft.h>
#include <TurnLeftLib/Utils/inlines.h>
using TurnLeft::Utils::HungryVector;
using TurnLeft::Utils::int_str_to_uchar;
using TurnLeft::Utils::uchar_to_str;

#include <string>
using std::string;

#include <algorithm>
using std::count;

#include <openssl/blowfish.h>

#include <assert.h>

enum PacketType
{
    DECRYPTED = 0, ENCRYPTED = 1, DO_TRANSLATION = 2, NO_TRANSLATION = 3
};

class PacketCenter
{
private:
    HungryVector<Packet*> packets[2];
    bool processed[2];
    BF_KEY* _key;
    void get_8_bytes(
        string in, CharArray* out, int& strpos, short& out_len);
    Packet* fill_packet(
        string in, int& start);
    string process_encryption(
        PacketType type, PacketType translation = DO_TRANSLATION);

public:
    PacketCenter(
        string input, PacketType inputType);
    ~PacketCenter();
    inline PacketCenter& giveKey(
        BF_KEY* key)
    {
        _key = key;
        return *this;
    }
    string encrypt();
    string decrypt(
        PacketType translation = DO_TRANSLATION);
};

#endif /* PACKETCENTER_H_ */
