#include "BlowfishKey.h"
#include "KeyDefinitions.h"
#include <stdlib.h>

#ifndef BF_RANDOM_BYTES
#define BF_RANDOM_BYTES

#define _BF_RANDOM_BYTE_0 50
#define _BF_RANDOM_BYTE_1 51
#define _BF_RANDOM_BYTE_2 52
#define _BF_RANDOM_BYTE_3 53
#define _BF_RANDOM_BYTE_4 54
#define _BF_RANDOM_BYTE_5 55
#define _BF_RANDOM_BYTE_6 56
#define _BF_RANDOM_BYTE_7 57
#define _BF_RANDOM_BYTE_8 58
#define _BF_RANDOM_BYTE_9 59
#define _BF_RANDOM_BYTE_10 60
#define _BF_RANDOM_BYTE_11 61
#define _BF_RANDOM_BYTE_12 62
#define _BF_RANDOM_BYTE_13 63
#define _BF_RANDOM_BYTE_14 64
#define _BF_RANDOM_BYTE_15 65
#endif

BlowfishKeyCreator::BlowfishKeyCreator()
{
    key = (KeyArray) malloc(sizeof(KeyArray) * 16);
    bf_key = std::make_shared<BF_KEY>();

    key[0] = (unsigned char) _BF_RANDOM_BYTE_0;
    key[1] = (unsigned char) _BF_RANDOM_BYTE_1;
    key[2] = (unsigned char) _BF_RANDOM_BYTE_2;
    key[3] = (unsigned char) _BF_RANDOM_BYTE_3;
    key[4] = (unsigned char) _BF_RANDOM_BYTE_4;
    key[5] = (unsigned char) _BF_RANDOM_BYTE_5;
    key[6] = (unsigned char) _BF_RANDOM_BYTE_6;
    key[7] = (unsigned char) _BF_RANDOM_BYTE_7;
    key[8] = (unsigned char) _BF_RANDOM_BYTE_8;
    key[9] = (unsigned char) _BF_RANDOM_BYTE_9;
    key[10] = (unsigned char) _BF_RANDOM_BYTE_10;
    key[11] = (unsigned char) _BF_RANDOM_BYTE_11;
    key[12] = (unsigned char) _BF_RANDOM_BYTE_12;
    key[13] = (unsigned char) _BF_RANDOM_BYTE_13;
    key[14] = (unsigned char) _BF_RANDOM_BYTE_14;
    key[15] = (unsigned char) _BF_RANDOM_BYTE_15;

    //KeyArray const_key =
    //		reinterpret_cast <KeyArray> (key);

    BF_set_key(bf_key.get(), 16, key);
}

BlowfishKeyCreator::BlowfishKeyCreator(const BlowfishKeyCreator& proto)
    : bf_key(proto.getKey())
{
    key = nullptr;
}

BlowfishKeyCreator::~BlowfishKeyCreator()
{
}



BF_KEY* BlowfishKeyCreator::getKey() const
{
    return bf_key.get();
}
