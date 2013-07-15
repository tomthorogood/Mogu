#ifndef SYNTAXDEF_H_
#define SYNTAXDEF_H_

#include <declarations.h>

struct SyntaxDef
{
    int             integer =0;
    std::string     str     =EMPTY;
    std::string     human   =EMPTY;
    
    SyntaxDef (int i_, std::string s_)
        : integer(i_), str(std::to_string(i_)), human(s_)
    {}

    inline operator std::string() const
    { return str; }

    inline operator std::string& () const
    { return str; }

    inline operator int () const
    { return integer; }

    inline operator int& () const
    { return integer; }
}

#endif
