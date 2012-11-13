/*
 * EmailManager.h
 *
 *  Created on: Nov 13, 2012
 *      Author: tom
 */

#ifndef EMAILMANAGER_H_
#define EMAILMANAGER_H_

#include <declarations.h>

class EmailManager
{
	std::string to_address;
	std::string subject;
	std::string message;

	uint8_t __status;
public:
	EmailManager()
	{
		__status =0;
		to_address = EMPTY;
		subject = EMPTY;
		message = EMPTY;
	}
	~EmailManager(){}

	inline void setMessage(const std::string& msg)
	{
		message = msg;
		__status |= 0x1;
	}

	inline void setSubject(const std::string& subj)
	{
		subject = subj;
		__status |= 0x2;
	}

	inline void setRecipient(const std::string& addr)
	{
		to_address = addr;
		__status |= 0x4;
	}

	inline bool send()
	{
		const uint8_t cpl = 0x1|0x2|0x4;
		if ( __status != cpl) return false;
		__status =0x0; //Reset for future use

		std::stringstream buffer;
		buffer << "echo " << message;
		buffer << " |mail -s \" ";
		buffer << subject << "\" ";
		buffer << to_address;
		std::string command = buffer.str();

		return system(command.c_str());
	}
};


#endif /* EMAILMANAGER_H_ */
