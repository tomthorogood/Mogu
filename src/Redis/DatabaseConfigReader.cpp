/*
 * DatabaseConfigReader.cpp
 *
 *  Created on: Apr 4, 2013
 *      Author: tom
 */
#include "DatabaseConfigReader.h"
#include "ContextMap.h"
#include <cassert>
#include <iostream>
#include <fstream>

namespace Application {

Context_Map* context_map =nullptr;

template <typename T>
bool str_contains(const std::string& s, T v)
{
    return s.find(v) != std::string::npos;
}

Prefix match_prefix (int& prefix_mask, const std::string& prefix)
{
    if (str_contains(prefix, "widgets"))
    {
        prefix_mask |= (int) Prefix::widgets;
        return Prefix::widgets;
    }
    else if (str_contains(prefix, "data"))
    {
        prefix_mask |= (int) Prefix::data;
        return Prefix::data;
    }
    else if (str_contains(prefix, "user"))
    {
        prefix_mask |= (int) Prefix::user;
        return Prefix::user;
    }
    else if (str_contains(prefix,"group"))
    {
        prefix_mask |= (int) Prefix::group;
        return Prefix::group;
    }
    else if (str_contains(prefix,"template"))
    {
        prefix_mask |= (int) Prefix::templates;
        return Prefix::templates;
    }
    else if (str_contains(prefix,"perspective"))
    {
        prefix_mask |= (int) Prefix::perspectives;
        return Prefix::perspectives;
    }
    else if (str_contains(prefix,"validators"))
    {
        prefix_mask |= (int) Prefix::validators;
        return Prefix::validators;
    }
    else if (str_contains(prefix,"policies"))
    {
        prefix_mask |= (int) Prefix::policies;
        return Prefix::policies;
    }
    else if (str_contains(prefix,"meta"))
    {
        prefix_mask |= (int) Prefix::meta;
        return Prefix::meta;
    }
    else return Prefix::__NONE__;
}

int extract_integer(const std::string& line)
{
    const std::string digits = "1234567890";
    size_t digit_start = line.find_first_of(digits);
    std::string digit = line.substr(digit_start);
    size_t digit_end = digit.find_first_not_of(digits);
    digit = digit.substr(0,digit_end);
    return atoi(digit.c_str());
}

std::string extract_host(const std::string& line)
{
    const std::string alpha_num = "abcdefghijklmnopqrstuvwxyz1234567890";
    size_t d = line.find(":");
    std::string tail = line.substr(d+1);
    size_t start = tail.find_first_of(alpha_num);
    tail = tail.substr(start);
    size_t end = tail.find_first_not_of(alpha_num);
    return tail.substr(0,end);
}

inline bool ignore_line(const std::string& line)
{
    return (str_contains(line,'#') || line.empty());
}

void load_database_contexts()
{
    extern Context_Map* context_map;
    if (context_map) return;
    int prefix_mask {};
    context_map = new Context_Map();

    std::ifstream buf;
    buf.open(DBCONFIG_FILE);
    assert(buf.is_open());

    std::string line {};
    
    while (!buf.eof())
    {
        getline(buf,line);
        if (ignore_line(line)) continue;
        else if (str_contains(line,'@'))
        {
            int port {};
            std::string host {};
            int db_num {};
            
            // Complete at 00000111 (7)
            // One bit is set each time one of the above is found.
            // This allows for any number of newlines or comments.
            uint8_t completion {};
            Prefix p {match_prefix(prefix_mask, line)};
            while ((completion != 7) && !buf.eof())
            {
                getline(buf,line);
                if (ignore_line(line)) continue;
                else if (str_contains(line,"port"))
                {
                    port = extract_integer(line);
                    completion |= 1;
                }
                else if (str_contains(line, "number"))
                {
                    db_num = extract_integer(line);
                    completion |= 2;
                }
                else if (str_contains(line, "host"))
                {
                    host = extract_host(line);
                    completion |= 4;
                }
            }
            assert(completion==7);
            Redis::Context* c = new Redis::Context();
            c->port = port;
            c->host = host;
            c->db_num = db_num;
            context_map->set(p,c);
            assert(context_map->get(p));
        }
    }
    buf.close();
    assert(prefix_mask >= max_prefix_mask);
    assert(context_map);
}

}//namespace Application
