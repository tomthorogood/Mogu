/*
 * DatabaseConfigReader.cpp
 *
 *  Created on: Apr 4, 2013
 *      Author: tom
 */
#include "DatabaseConfigReader.h"
#include "ContextMap.h"
#include <cassert>

namespace Application {


Prefix matchPrefix(int& PREFIX_MASK, const std::string& prefix)
{
    if (prefix.find("widgets") != std::string::npos) {
        PREFIX_MASK |= (int) Prefix::widgets;
        return Prefix::widgets;
    }
    if (prefix.find("data") != std::string::npos) {
        PREFIX_MASK |= (int) Prefix::data;
        return Prefix::data;
    }
    if (prefix.find("user") != std::string::npos) {
        PREFIX_MASK |= (int) Prefix::user;
        return Prefix::user;
    }
    if (prefix.find("group") != std::string::npos)
    {
        PREFIX_MASK |= (int) Prefix::group;
        return Prefix::group;
    }
    if (prefix.find("templates") != std::string::npos)
    {
        PREFIX_MASK |= (int) Prefix::templates;
        return Prefix::templates;
    }
    if (prefix.find("validators") != std::string::npos)
    {
        PREFIX_MASK |= (int) Prefix::validators;
        return Prefix::validators;
    }
    if (prefix.find("policies") != std::string::npos) {
        PREFIX_MASK |= (int) Prefix::policies;
        return Prefix::policies;
    }
    else {
        PREFIX_MASK |= (int) Prefix::meta;
        return Prefix::meta;
    }

}

int extractInteger(const std::string& line) {
    auto digit_start = line.find_first_of("1234567890");
    auto digit = line.substr(digit_start);
    auto digit_end = digit.find_first_not_of("1234567890");
    digit = digit.substr(0,digit_end);
    const char* c_digit = digit.c_str();
    int i_digit = atoi(c_digit);
    return i_digit;
}

std::string getHost(const std::string& line) {
    auto delim = line.find(":");
    auto tail = line.substr(delim+1);
    /*Allow numbers, letters, and periods in the hostname. This may change.
     * Periods may not start an IP address, so ignore those at the start search.
     */
    auto start = tail.find_first_of("abcdefghijklmnopqrstuvwxyz1234567890");
    tail = tail.substr(start);
    auto end = tail.find_first_not_of("abcdefghijklmnopqrstuvwxyz1234567890.");
    return tail.substr(0,end);
}

ContextMap* loadDatabaseContexts() {
    int PREFIX_MASK = 0;
    ContextMap* contextMap = new ContextMap();
    
    std::ifstream infile;
    infile.open(DBCONFIG_FILE);
    if(!infile.is_open()) {
        std::cout << "Warning: No database config file found! Expect a "
            << "segfault soon..." << std::endl;
        delete contextMap;
        return nullptr;
    }

    /* Initialize the line string, which will be used to hold
     * each line of the config file in turn. */
    std::string line;

    while (!infile.eof())
    {
        getline(infile,line);

        // Ignore commented lines:
        if (line.find('#') != std::string::npos) continue;
        if (line == "") continue; //and blank lines

        //Enter the context loop
        if (line.find('@') != std::string::npos)
        {
            int port;
            std::string host;
            int dbnum;
            // Complete at 00000111 (7)
            // One bit is set each time one of the above is found.
            // This allows for any number of newlines or comments.
            uint8_t completion =0;
            Prefix prefix = matchPrefix(PREFIX_MASK, line);

            while (completion != 7 && !infile.eof())
            {

                getline(infile,line);
                // Explicitly consume commented lines, as they may
                // have the words 'port','host', or 'number' in them.
                if (line.find('#') != std::string::npos) continue;
                if (line == "") continue;

                else if (line.find("port") != std::string::npos)
                {
                    port = extractInteger(line);
                    completion |= 1;
                }
                else if (line.find("number") != std::string::npos)
                {
                    dbnum = extractInteger(line);
                    completion |= 2;
                }
                else if (line.find("host") != std::string::npos)
                {
                    host = getHost(line);
                    completion |= 4;
                }
            }
            //!\todo Throw an error if a context not fully defined.
            if (completion < 7)
            {
                delete contextMap;
                return nullptr;
            }
            contextMap->set(prefix, new Redis::Context(port, host.c_str(), dbnum));
            assert(contextMap->get(prefix) != nullptr);
        }
    }
    infile.close();
    //!\todo Throw an error if not everything was loaded.
    if (PREFIX_MASK < MAX_PREFIX_MASK)
        return nullptr;
    assert(contextMap != 0x0);
    return contextMap;
}

}
