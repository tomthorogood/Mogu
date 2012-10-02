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
 * persistent within the user session. The data held here is unique
 * to each user. This can make data available globally to the application
 * instance without exposing it anywhere else.
 */
namespace Application
{
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

	bool metaKeyConfigured(std::string key);
	std::string getMetaValue(std::string key);
}


#endif /* STATIC_H_ */
