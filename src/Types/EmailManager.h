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
    std::string to_address  = EMPTY;
    std::string subject     = EMPTY;
    std::string message     = EMPTY;

    uint8_t status        = 0;
public:
    EmailManager()
    {
    }
    ~EmailManager()
    {
    }

    inline void setMessage(
        const std::string& msg)
    {
        message = msg;
        status |= 0x1;
    }

    inline void setSubject(
        const std::string& subj)
    {
        subject = subj;
        status |= 0x2;
    }

    inline void setRecipient(
        const std::string& addr)
    {
        to_address = addr;
        status |= 0x4;
    }

    inline bool send()
    {
        const uint8_t cpl = 0x1 | 0x2 | 0x4;
        if (status != cpl) return false;
        status = 0x0;    //Reset for future use

        std::stringstream buffer;
        buffer << "echo " << "\"" << message << "\"";
        buffer << " |mail -s \" ";
        buffer << subject << "\" ";
        buffer << to_address;
        std::string command = buffer.str();

        return system(command.c_str());
    }
};

#endif /* EMAILMANAGER_H_ */
