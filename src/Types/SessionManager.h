/*
 * SessionManager.h
 *
 *  Created on: Oct 28, 2012
 *      Author: tom
 */

#ifndef SESSIONMANAGER_H_
#define SESSIONMANAGER_H_

#include <declarations.h>

class SessionManager
{
private:
	void __populate_previous_sessions();
	std::vector <std::string> __previous_sessions;

public:
	SessionManager(std::string sessionid="global");

	std::vector <std::string>& getPreviousSessions() {
		return __previous_sessions;
	}

	/*!\brief Searches the user's linked session list for the first instance
	 * of "key". This takes care of any hasing, so should only receive plaintext
	 * keys.
	 * @param key
	 * @return The full node name at which this key exists.
	 */
	std::string findKeyNode (std::string key);

	/*!\brief Searches the user's linked session list for the first instance
	 * of "key", and returns its value.
	 * @param key The plaintext key being sought.
	 * @param arg (optional) An argument (field, index) for the get operation.
	 * @return The value stored at that key.
	 */
	std::string findValue(std::string key, std::string arg=EMPTY);
};


#endif /* SESSIONMANAGER_H_ */
