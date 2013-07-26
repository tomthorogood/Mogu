#include <hash.h>
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>

namespace Hash {

std::string toHash(const std::string& str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash,&sha256);
    std::stringstream buf;
    for (int i =0; i < SHA256_DIGEST_LENGTH; ++i)
        buf << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    return buf.str();
}

}    //namespace hash
