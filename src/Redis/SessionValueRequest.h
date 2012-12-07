/*
 * SessionValueRequest.h
 *
 *  Created on: Oct 28, 2012
 *      Author: tom
 */

#ifndef SESSIONVALUEREQUEST_H_
#define SESSIONVALUEREQUEST_H_

#include <declarations.h>
#include <Redis/StoragePolicyLookup.h>

namespace Redis {

class SessionValueRequest
{
    Mogu* appl;
    std::string node_name;
    std::string retrieved_value;
    void process_request(
        std::vector<std::string>& session_ids, const std::string& key,
        const std::string& arg);

    StoragePolicyLookup lookup;
    std::string keyhash;

    inline std::string build_session_node(
        const std::string& session, const std::string& key)
    {
        return "s." + session + "." + key;
    }
public:
    /* NOTE: "key" should be plaintext. Arg should already be hashed, if
     * applicable! We need the key in plaintext to lookup the policy. We need
     * the arg pre-hashed because we don't really know otherwise whether or not
     * it needs to be. The key will always be hashed within a user namespace.
     */
    SessionValueRequest(
        const std::string& key, std::string arg = EMPTY, std::string endpoint =
            EMPTY);
    SessionValueRequest(
        std::vector<std::string>& session_ids, const std::string& key,
        std::string arg = EMPTY);

    inline const std::string& getNode()
    {
        return node_name;
    }
    inline const std::string& getValue()
    {
        return retrieved_value;
    }

};

}    //namespace Redis

#endif /* SESSIONVALUEREQUEST_H_ */
