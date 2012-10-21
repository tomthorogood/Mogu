/*
 * SessionHandler.h
 *
 *  Created on: Oct 19, 2012
 *      Author: tom
 */

#include <declarations.h>

#ifndef SESSIONHANDLER_H_
#define SESSIONHANDLER_H_

/* The session handler is bound to a specific instance of a Mogu application,
 * meaning each user has their own. This is used to look up session information,
 * handle encryption, and so forth.
 */

namespace Sessions{

class SessionHandler
{
private:
	std::vector <std::string> __linked_sessions;

	/*!\brief Finds all user session ids.
	 * Populates the __linked_sessions vector in order to avoid doing
	 * the backwards iterative loop to query the database every time
	 * information is requested from user specific sessions.
	 */
	void gather_linked_sessions();

	/*!\brief Purges expired auth tokens and allows them to be reused. */
	void cleanup_auth_tokens();

public:
	SessionHandler(const std::string& sessionid=std::string("global"));
	~SessionHandler();

	/*!\brief Writes information to the database in the current user
	 * session
	 * @param plaintext_storage_node The name of the node in the database where
	 * the data is to be stored. This should not have any prefix/suffix info, as
	 * that will be handled by the function. It must be parseable Mogu syntax.
	 * @param data_to_store
	 * @return
	 */
	bool store(Redis::RedisStorageRequest& request);

	Nodes::NodeValue retrieve (
			const std::string& plaintext_storage_node
			, const::std::string& arg = std::string(EMPTY));
};

}//namespace Sessions
#endif /* SESSIONHANDLER_H_ */
