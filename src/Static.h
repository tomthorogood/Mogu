/*
 * Static.h
 *
 *  Created on: Jul 29, 2012
 *      Author: tom
 */

#ifndef STATIC_H_
#define STATIC_H_

#include <declarations.h>

/*!\brief The Application namespace houses a library of functions that are
 * persistent within the user session, but the data held here is unique
 * to each user. This can make data available globally to the application
 * instance without exposing it anywhere else.
 */
namespace Application
{
	/*!\brief Returns a pointer to the Mogu object.
	 * This is mostly used to ensure there hasn't been memory corruption when
	 * validating sessions. It also allows widgets to communicate with the
	 * application itself.
	 * @return a pointer to the application instance
	 */
	Mogu* mogu();

	/*!\brief Called when an application instance starts. The Mogu instance
	 * will tell the namespace that it exists, and hand over a pointer to itself
	 * which can be used anywhere else in the application.
	 * @param application A pointer to a Mogu instance.
	 */
	void defineMogu(Mogu* application);

	/*!\brief Requests the user session id. By default, this will return 'global'
	 * unless the 'change_session' action has taken place.
	 * @param wtsession A pointer to the mogu session. This makes sure that the
	 * application requesting the session is the same application which set up
	 * the session, and averts disaster should something untoward happen to
	 * memory.
	 * @return The user's current working session id.
	 */
	std::string requestSessionID(std::string wtsession);

	/*!\brief Requests the user's authentication token (mostly used when
	 * instantiating dynamic widgets). The user's session ID is required to
	 * access this to prevent corruption.
	 * @param sessionid The user's session id, which must match what the
	 * Application *thinks* the user's session id is.
	 * @return The user's encrypted authentication token.
	 */
	std::string requestAuthToken(std::string sessionid);

	/*!\brief Sets the user's authentication token. */
	void setAuthToken(std::string auth);

	/*!\brief Sets the Wt session ID automatically generated when a connection
	 * is made.
	 * @param session
	 */
	void setWtSession(std::string session);

	/*!\brief Sets the user session id (only used if the change_session action
	 * takes place.
	 * @param session
	 */
	void setSessionID(std::string session);

	/*!\brief Encrypts a string of any variable length. It is done in the
	 * application namespace to avoid constantly created and deleting the
	 * server's encryption key, which could lead to indecent exposure.
	 * @param data Any string
	 * @return The encrypted version of the string that can only be decrypted
	 * on this server.
	 */
	std::string encrypt(std::string data);

	/*!\brief Temporarily stores data in this globally accessible namespace.
	 * Once this data is retrieved, it is no longer accessible.
	 *
	 * @param name The name of the data
	 * @param value The data value
	 */
	void slotStorage(std::string name, std::string value);

	/*!\brief Retrieves temporarily stored data. Once retrieved, this data is
	 * no longer accessible from anywhere.
	 * @param name The data key.
	 * @param wtSession Ensures that the application instance is the same before
	 * allowing the data to be accessed.
	 * @return
	 */
	std::string retrieveSlot (std::string name, std::string wtSession);

}


#endif /* STATIC_H_ */
