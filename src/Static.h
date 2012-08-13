/*
 * Static.h
 *
 *  Created on: Jul 29, 2012
 *      Author: tom
 */

#ifndef STATIC_H_
#define STATIC_H_

#include <declarations.h>

namespace Application
{
	Mogu* mogu();
	void defineMogu(Mogu* application);
	std::string requestSessionID(std::string auth);
	void setAuthToken(std::string auth);
	void setWtSession(std::string session);
	void setSessionID(std::string session);
	bool handshake(std::string wtSession);
	std::string encrypt(std::string data);
	void slotStorage(std::string name, std::string value);
	std::string retrieveSlot (std::string name, std::string wtSession);
}


#endif /* STATIC_H_ */
