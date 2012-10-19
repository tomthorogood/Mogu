/*
 * RedisStorageRequest.h
 *
 *  Created on: Oct 19, 2012
 *      Author: tom
 */

#ifndef REDISSTORAGEREQUEST_H_
#define REDISSTORAGEREQUEST_H_

#include <declarations.h>

/*!\brief This struct holds information about how a value is to be
 * stored in the database. All writes to the database should use this in order
 * to keep the database integrity in tact.
 */

namespace Redis{

class RedisStorageRequest
{
	/*!\brief The plaintext, unprocessed 'meat' of the node name. */
	const std::string& node_body;

	/*!\brief The session id where the data will be stored. */
	std::string& session_id;

	/*!\brief Whether or not the data should first be encrypted. */
	bool encrypted;

	/*!\brief The command to be executed against the database.*/
	std::string command;

	/*!\brief The value to be stored. */
	Nodes::NodeValue* value;
public:
	RedisStorageRequest(const std::string& nodeBody);

	/*!\brief One may set values manually, or one may give the object a
	 * policy name to look up.
	 * @param policyName The name of the policy that is to be searched.
	 * @return Whether or not the lookup was successful.
	 */
	bool policyLookup(const std::string& policyName);

	void setSessionID(std::string& session_id);

	/*!\brief Requests that the database accept the command. Returns whether
	 * or not the command was successful.
	 */
	bool execute();

};
}//namespace Redis
#endif /* REDISSTORAGEREQUEST_H_ */
