#ifndef QUERYBLUEPRINT_H_
#define QUERYBLUEPRINT_H_

namespace Redis {
struct QueryBlueprint {
    std::string     query = EMPTY;
    uint8_t         specs = 0;

    inline void appendString(bool final=true) 
    {
        query += "%s";
        if (!final) query += " ";
        specs <<= 1;
        specs += 1;
    }

    inline void appendLiteral(const std::string& literal, bool final=true)
    {
        query += literal;
        if (!final) query += " ";
    }

    inline void appendInt(bool final=true)
    {
        query += "%d";
        if (!final) query += " ";
        specs <<= 1;
    }

    inline size_t nargs() 
    {
        if (!specs) return 0;

        uint8_t tmp = specs;
        size_t n = 1;

        while (tmp >>=1)
            n +=1;
        return n;
    }

};
}//namespace Redis

#endif
