#ifndef SYNTAXDEF_H_
#define SYNTAXDEF_H_

#include <declarations.h>
#include "NodeValue.h"

struct SyntaxDef
{
    int             integer =0;
    std::string     str     =EMPTY;
    std::string     human   =EMPTY;
    
    SyntaxDef (int i_, std::string s_)
        : integer(i_), str(std::to_string(i_)), human(s_)
    {}

    SyntaxDef(const SyntaxDef& other)
        : 
            integer(other.integer)
            , str(other.str)
            , human(other.human)

    {}

    inline operator std::string() const
    { return str; }

    inline operator int () const
    { return integer; }

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

    inline SyntaxDef& operator= (const SyntaxDef& other)
    {
        integer = other.integer;
        str = other.str;
        human = other.human;
    }

};

#endif
