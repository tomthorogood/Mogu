/*
 * Encryption.h
 *
 *  Created on: Oct 24, 2012
 *      Author: tom
 */

#ifndef ENCRYPTION_H_
#define ENCRYPTION_H_

#include <declarations.h>
#include <crypt/PacketCenter.h>

namespace Security
{
std::string encrypt(std::string decrypted);
std::string decrypt(std::string encrypted, PacketType translation=DO_TRANSLATION);
}


#endif /* ENCRYPTION_H_ */
