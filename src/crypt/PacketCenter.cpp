/*
 * PacketCenter.cpp
 *
 *  Created on: Jun 21, 2012
 *      Author: tom
 */

#include "PacketCenter.h"
#include <iostream>
#include <sstream>
#include <stdlib.h>

PacketCenter::PacketCenter(
    std::string input, PacketType inputType)
{
    /* By default the encryption key is set to null, for obvious reasons. */
    _key = 0;
    /* Initialize the vectors. */
    packets[ENCRYPTED].resize(1);
    packets[DECRYPTED].resize(1);
    processed[0] = false;
    processed[1] = false;

    HungryVector<Packet*>* packet_vector = 0;

    /* If we're being handed a plaintext string */
    if (inputType == DECRYPTED) {
        /* Alias the packet vector we're currently work with */
        packet_vector = &packets[DECRYPTED];
        unsigned int input_len = input.length();

        /* Pad the input to have a length evenly divisible by eight. */
        while (input_len % Packet::SIZE != 0) {
            input.append(" ");
            input_len++;
        }
        /* Determine the number of paackets that will be generated
         * from the input.
         */
        unsigned int num_packets = input_len / Packet::SIZE;

        /*Split the input into chunks and place them into packets,
         * all of which go into the DECRYPTED packets vector.
         */
        for (unsigned int p = 0; p < num_packets; p++) {
            int start = p * Packet::SIZE;
            std::string substring = input.substr(start, Packet::SIZE);

            Packet* pkt = new Packet(substring);
            packet_vector->add(pkt);
        }

        /* Get rid of extra slots from the double function. */
        packet_vector->trim();

        /* Make sure that we don't have the option to process this again. */
        processed[DECRYPTED] = true;
    }
    else {

        /* Grab the correct vector. */
        packet_vector = &packets[ENCRYPTED];

        /* Get rid of the extra space that should be at the end of the string,
         * taking any other accidental extra spaces with it.
         */
        TurnLeft::Utils::trimchar(input);

        /* There will be one space between each set, therefore, there is one
         * more set than spaces.
         */
        int num_chars = count(input.begin(), input.end(), ' ') + 1;

        /* This should be a number divisble by nine already, because
         * Blowfish null terminates packets.
         * If this assertion fails, it's because somebody
         * was tampering with the data...
         */
        assert(num_chars % (Packet::SIZE+1) == 0);

        int num_packets = num_chars / (Packet::SIZE + 1);

        /* This variable will be modified by the fill_packet method */
        int start = 0;
        for (int p = 0; p < num_packets; p++) {
            /* Extracts the next eight char sets from the string,
             * converts the sets into an unsigned char, and assigns
             * that char to a spot in the CharArray that will
             * be given to the packet.
             */
            Packet* pkt = fill_packet(input, start);
            packet_vector->add(pkt);
        }

        /* Trim any extra slots generated by the double method. */
        packet_vector->trim();

        /* Make sure we are not able to process encrypted data again. */
        processed[ENCRYPTED] = true;
    }
}

Packet* PacketCenter::fill_packet(std::string input, int& start)
{
    /* `bytes` refers to the number of bytes currently discovered. */
    int bytes = 0;

    /* Holds the length between `start` and the last space demarking the
     * end of the last character set.
     */
    int advance = 0;

    /* Iterate over the string until eight bytes are found. Note that
     * we've already asserted that the string has a number of bytes
     * encoded divisible by nine.
     */
    for (unsigned int i = start; bytes < (Packet::SIZE + 1); i++) {
        if (i >= input.length()) break;
        if (input.at(i) == ' ') {
            bytes++;
        }
        advance++;
    }
    /* Extract only eight bytes of ciphertext. */
    std::string sub = input.substr(start, advance);

    /* Create a CharArray out of the string, padding it with one extra
     * byte to avoid a segfault when handing to the Blowfish methods.
     */
    CharArray _sub = int_str_to_uchar<Packet::SIZE + 1>(sub);

    /* Increment `start` for the next call to this method. */
    start += advance;

    /* Finally, place the CharArray into a new Packet object. */
    return new Packet(_sub);
}

std::string PacketCenter::process_encryption(
    PacketType type, PacketType translation)
{
    PacketType other_type = type == ENCRYPTED ? DECRYPTED : ENCRYPTED;
    HungryVector<Packet*>* read = &packets[type];
    HungryVector<Packet*>* write = &packets[other_type];

    /* Guard against repeated calls, and makes sure that a key
     * was handed to this object.
     */
    if (_key == 0 || processed[other_type]) return "";

    /* Determine how many packets we'll be sending for encryption. */
    int num_to_process = read->size();

    /* Iterate over the packets we've created */
    for (int i = 0; i < num_to_process; i++) {
        /* Allocate memory for the CharArray that will hold the
         * encrypted ciphertext.
         */
        CharArray out = (CharArray) calloc(Packet::SIZE + 1, sizeof(char));

        /* Extract the CharArray from the packet to be encrypted. */
        CharArray in = read->at(i)->getData();

        /* Send the data for encryption at last! */
        BF_ecb_encrypt(in, out, _key, (int) other_type);

        /* As always, trim the vector we've created to guard against extra
         * data mucking things up, and make sure that we are not allowed
         * to call this method again.
         */
        write->add(new Packet(out));
    }
    write->trim();
    std::stringstream strm;
    int num_packets = write->size();
    for (int i = 0; i < num_packets; i++) {
        std::string str;
        if (other_type == ENCRYPTED) {
            str = write->at(i)->int_str();
        }
        else {
            str = write->at(i)->str();
        }
        int len = str.length();
        for (int c = 0; c < len; c++) {
            char ch = str.at(c);
            if (ch == ' ') {
                char o = (translation == DO_TRANSLATION) ? '_' : ' ';
                strm << o;
            }

            else {
                strm << ch;
            }
        }
    }
    return strm.str();
}

std::string PacketCenter::decrypt(
    PacketType translation)
{
    return process_encryption(ENCRYPTED, translation);
}

std::string PacketCenter::encrypt()
{
    return process_encryption(DECRYPTED, DO_TRANSLATION);
}

PacketCenter::~PacketCenter()
{
    for (int v = 0; v < 2; v++) {
        HungryVector<Packet*>* vec = &packets[v];
        int sz = vec->size();
        for (int p = 0; p < sz; p++) {
            if (vec->at(p) != 0) {
                delete vec->at(p);
            }
        }
    }
}
