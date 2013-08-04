/*
 * Encryption.h
 *
 *  Created on: Oct 24, 2012
 *      Author: tom
 */

#ifndef ENCRYPTION_H_
#define ENCRYPTION_H_


#include "../crypt/PacketCenter.h"
#include <string>

namespace Security {
std::string encrypt(
    std::string decrypted);
std::string decrypt(
    std::string encrypted, PacketType translation = DO_TRANSLATION);

inline bool isEncrypted(const std::string& str)
{
    for (size_t i = 0; i < str.size(); ++i)
    {
        if (! (isdigit(str[i])||'_'==str[i])) return false;
    }
    return true;
}

}

#endif /* ENCRYPTION_H_ */
