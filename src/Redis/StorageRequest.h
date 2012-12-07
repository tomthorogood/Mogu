/*
 * RedisStorageRequest.h
 *
 *  Created on: Oct 19, 2012
 *      Author: tom
 */

#ifndef REDISSTORAGEREQUEST_H_
#define REDISSTORAGEREQUEST_H_

#include <declarations.h>
#include <Parsers/MoguScript_Tokenizer.h>
#include <Types/NodeValue.h>

/*!\brief This struct holds information about how a value is to be
 * stored in the database. All writes to the database should use this in order
 * to keep the database integrity in tact.
 */

namespace Redis {

class StorageRequest
{
    /*!\brief The message containing the string which will parse to the policy.*/
    std::string& destination;

    /*!\brief A tokenizer for the input destination. */
    Parsers::MoguScript_Tokenizer tokenizer;

    /*!\brief The bare bones name of the policy */
    std::string policy_token;

    /*!\brief The session id where the data will be stored. */
    std::string session_id;

    /*!\brief The command to be executed against the database.*/
    std::string command;
    std::string node_body;
    std::string arg;
    std::string f_value;

    /*!\brief The value to be stored. */
    NodeValue& value;

    void __init__();

    StoragePolicyLookup* lookup;
    bool policyLookup();

public:

    StorageRequest(
        std::string& destination, NodeValue& value);
    ~StorageRequest();
    /*!\brief One may set values manually, or one may give the object a
     * policy name to look up.
     * @param policyName The name of the policy that is to be searched.
     * @return Whether or not the lookup was successful.
     */

    void build_command();

    /*!\brief Requests that the database accept the command. Returns whether
     * or not the command was successful.
     */
    bool execute();

};
}    //namespace Redis
#endif /* REDISSTORAGEREQUEST_H_ */
