/*
 * StoragePolicyLookup.h
 *
 *  Created on: Oct 28, 2012
 *      Author: tom
 */

#ifndef STORAGEPOLICYLOOKUP_H_
#define STORAGEPOLICYLOOKUP_H_

#include <Types/syntax.h>

namespace Redis {

class StoragePolicyLookup
{
    uint8_t __flags;
    enum Flags
    {
        exists = 0x1, encrypted = 0x2, set_append = 0x4
        //datatype = 3 bits
        ,
        datatype = 0x20
        //storagetype = 2 bits
        ,
        storagetype = 0x80
    };
    std::string getHashEntry(const std::string& node, const std::string& field);
public:

    StoragePolicyLookup(
        const std::string& policyName);

    inline std::string buildPolicyNode(
        const std::string& policyName) const
    {
        return "policies." + policyName;
    }

    const uint16_t getProperties()
    {
        return __flags;
    }

    inline bool isAppendMode()
    {
        return __flags & set_append;
    }
    inline bool policyExists()
    {
        return __flags & exists;
    }
    inline bool isEncrypted()
    {
        return __flags & encrypted;
    }
    inline Tokens::MoguTokens getDataType()
    {
        uint8_t masked = __flags & 0x38;
        masked >>= 3;
        return (Tokens::MoguTokens) masked;
    }

    inline Tokens::MoguTokens getStorageType()
    {
        uint8_t masked = __flags & 0xC0;
        masked >>= 6;
        return (Tokens::MoguTokens) masked;
    }
};

}    //namespace Redis

#endif /* STORAGEPOLICYLOOKUP_H_ */
