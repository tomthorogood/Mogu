#include <openssl/blowfish.h>
#include "Encryption.h"
#include "BFString.h"
#include "../crypt/BlowfishKey.h"
#include <cassert>
#include <iostream>

namespace Security {

namespace {
    void cpystr (const std::string& in, unsigned char* out)
    {
        int sz = in.size();
        for (int i =0; i < sz; ++i)
        {
            out[i] = in[i];
        }
    }

    void trim_padding(std::string& str)
    {
        size_t cl_brace = str.find_first_of('}');
        std::string orig_size = str.substr(1,cl_brace-1);
        int sz = atoi(orig_size.c_str());
        str = str.substr(cl_brace+1,sz);
    }
}

std::string process_packet (const std::string& packet, int method)
{
    BlowfishKeyCreator k;
    BF_KEY* key = k.get_key();
    unsigned char* input = 
        (unsigned char*) calloc(packet.size()+1,sizeof(char));
    unsigned char* output = 
        (unsigned char*) calloc(packet.size()+1,sizeof(char));

    cpystr(packet,input);


    BF_ecb_encrypt(input,output,key,method);

    std::string ret;

    for (size_t i = 0; i < packet.size(); ++i)
    {
        ret += output[i];
    }

    free(input);
    free(output);

    return ret;
}

std::string process_string (const std::string& input, int method)
{
    std::string ret ="";
    BFString bf_str(input, method==BF_DECRYPT);
    while (bf_str)
        ret += process_packet(bf_str.yield(), method);
    if (method==BF_DECRYPT) trim_padding(ret);
    return ret;
}

std::string encrypt(const std::string& str)
{
    std::string e = process_string(str, BF_ENCRYPT);
    return e;
}

std::string decrypt(const std::string& str)
{
    std::string d = process_string(str, BF_DECRYPT);
    return d;
}

}//namespace Securit
