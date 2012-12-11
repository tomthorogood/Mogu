/*
 * Packet.cpp
 *
 *  Created on: Jun 21, 2012
 *      Author: tom
 */

#include "Packet.h"
#include <TurnLeftLib/Utils/inlines.h>
using TurnLeft::Utils::HungryVector;
using TurnLeft::Utils::int_str_to_uchar;
using TurnLeft::Utils::str_to_uchar;
using TurnLeft::Utils::uchar_to_int_str;
using TurnLeft::Utils::uchar_to_str;

using std::string;

Packet::Packet(
    CharArray input)
{
    data = input;
}

Packet::Packet(
    c_str input)
{
    string _input_(input);
    data = str_to_uchar(_input_);
}

Packet::Packet(
    string input)
{
    data = str_to_uchar(input);
}

string Packet::str() const
{
    return uchar_to_str(data);
}

string Packet::int_str() const
{
    return uchar_to_int_str<SIZE + 1>(data);
}

Packet::~Packet()
{
    if (data != NULL) {
        free(data);
    }
}

