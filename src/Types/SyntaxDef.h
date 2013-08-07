#ifndef SYNTAXDEF_H_
#define SYNTAXDEF_H_

#include "Node_Value.h"
#include <string>

struct Syntax_Def
{
    int integer {};
    std::string str{}
    std::string human

    constexpr Syntax_Def (int i_, const char* s_, const char* h_)
        : integer(i_)
        , str(s_)
        , human(h_)
    {}

    constexpr Syntax_Def(const Syntax_Def& other)
        : integer(other.integer)
        , str(other.str)
        , human(other.human)

    {}

    inline operator std::string() const
        { return str; }

    constexpr inline operator int const&() const
        { return integer; }

    inline bool operator== (const Syntax_Def& other) const
        { return other.integer == integer; }

    inline bool operator== (const Node_Value& other) const
    { 
        if (other.is_int()) return other==integer;
        else if (other.is_string())
            return (other.get_string() == str)
                || (other.get_string() == human);
        else return false;
    }

    inline bool operator==(const int& other) const
        { return other == integer; }

    inline Syntax_Def& operator= (const Syntax_Def& other)
    {
        integer = other.integer;
        str = other.str;
        human = other.human;
        return *this;
    }

};

#endif
