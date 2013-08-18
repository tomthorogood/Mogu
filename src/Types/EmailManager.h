/*
 * Email_Manager.h
 *
 *  Created on: Nov 13, 2012
 *      Author: tom
 */

#ifndef EMAILMANAGER_H_
#define EMAILMANAGER_H_

#include <string>
#include <sstream>

class Email_Manager
{
    std::string to_address {};
    std::string subject {};
    std::string message {};

    uint8_t status {};
public:
    Email_Manager() {}
    ~Email_Manager() {}

    inline void set_message (const std::string& msg)
    {
        message = msg;
        status |= 0x1;
    }

    inline void set_subject (const std::string& s)
    {
        subject = s;
        status |= 0x2;
    }

    inline void set_recipient (const std::string& r)
    {
        to_address = r;
        status |= 0x4;
    }

    inline bool send()
    {
        constexpr uint8_t complete = 0x1|0x2|0x4;
        if (status != complete) return false;
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
