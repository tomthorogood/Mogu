#ifndef BFSTRING_H_
#define BFSTRING_H_

#include "Encryption.h"
#include <string>
#include <queue>
#include <sstream>
#include <cassert>
#include <iostream>
class BFString
{
public:
    BFString(std::string str, bool padded=false)
    {
        // Always ensure at least one character of padding.
        std::string s;
        if (!padded)
        {
            s = padinfo(str) + str;
            do {
                s += " ";
            } while (s.size() % BF_SIZE_);
        } else { 
            s = str;
        }


        for (size_t i=0; i < s.size();++i)
        {
            std::string sub = s.substr(i,BF_SIZE_);
            q.push(sub);
            i+=(BF_SIZE_-1);
        }

    }


    operator bool() { return static_cast<bool>(q.size()); }

    std::string yield()
    {
        std::string ret = q.front();
        q.pop();
        return ret;
    }

private:
    std::string padinfo(const std::string& str)
    {
        std::stringstream buf;
        buf << '{';
        buf << str.size() << '}';
        return buf.str();
    }
    std::queue <std::string> q;
};

#endif // BFSTRING_H_
