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
	std::string node_body;

	/*!\brief The session id where the data will be stored. */
	std::string& session_id;

	/*!\brief Whether or not the data should first be encrypted. */
	bool encrypted;

	/*!\brief The command to be executed against the database.*/
	std::string command;

	/*!\brief The value to be stored. */
	Nodes::NodeValue* value;

	std::string field;

	uint8_t ready_state;

	Enums::SubmissionPolicies::StorageMode mode;
	Enums::SubmissionPolicies::DataWrapping dtype;
	Enums::SubmissionPolicies::StorageType nodetype;

	inline bool ready() {
		// In order for the last bit to by set, all others must be set.
		if ( (ready_state | bit_ready_chk) == 0xFF)
			ready_state |= 0xFF;
		return ready_state == 0xFF;
	}

	enum statebits {
		bit_node_body 	=0x1
		,bit_session_id =0x2
		,bit_nodetype	=0x4
		,bit_in_value	=0x8
		,bit_mode		=0x10
		,bit_dtype		=0x20
		,bit_command	=0x40
		,bit_ready_chk	=0x80
	};
public:
	RedisStorageRequest(std::string nodeBody);

	/*!\brief One may set values manually, or one may give the object a
	 * policy name to look up.
	 * @param policyName The name of the policy that is to be searched.
	 * @return Whether or not the lookup was successful.
	 */
	bool policyLookup(const std::string& policyName);

	inline void setSessionID(std::string& sessionid)
	{
		session_id = sessionid;
		ready_state |= bit_session_id;
	}

	inline void setInput(Nodes::NodeValue* v)
	{
		value = v;
		ready_state |= bit_in_value;
	}

	void build_command();

	/*!\brief Requests that the database accept the command. Returns whether
	 * or not the command was successful.
	 */
	bool execute();

};
}//namespace Redis
#endif /* REDISSTORAGEREQUEST_H_ */
