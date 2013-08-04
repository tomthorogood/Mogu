#ifndef SYNTAXDEF_H_
#define SYNTAXDEF_H_

#include "NodeValue.h"
#include <string>

struct SyntaxDef
{
    int             integer =0;
    const char*     str     ="";
    const char*     human   ="";
    
    constexpr SyntaxDef (int i_, const char* s_, const char* h_)
        : integer(i_), str(s_), human(h_)
    {}

    constexpr SyntaxDef(const SyntaxDef& other)
        : 
            integer(other.integer)
            , str(other.str)
            , human(other.human)

    {}

    inline operator std::string() const
    { return str; }

    constexpr inline operator int const&() const
    { return integer; }

//    inline operator int() const
//    { return integer;}

    inline bool operator== (const SyntaxDef& other) const
    { return other.integer == integer; }

    inline bool operator== (const NodeValue& other) const
    { 
        if (other.isInt())
            return (int) other == integer;
        else if (other.isString())
            return ((std::string) other == str)
                || ((std::string) other == human);
        else return false;
    }

    inline bool operator==(const int& other) const
    { return other == integer; }

    inline SyntaxDef& operator= (const SyntaxDef& other)
    {
        integer = other.integer;
        str = other.str;
        human = other.human;
        return *this;
    }

};

#endif
