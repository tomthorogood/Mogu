/*
 * MoguNode.h
 *
 *  Created on: Dec 9, 2012
 *      Author: tom
 */

#ifndef MOGUNODE_H_
#define MOGUNODE_H_

class FixedString
{
protected:
    std::string __str;
    inline std::string assertEndsWith (const std::string& suffix) const
    {
        size_t suf_len = suffix.length();
        size_t str_len = __str.length();
        size_t incl_index = __str.find(suffix);
        if (incl_index == std::string::npos) return __str + suffix;

        return
            ((str_len - suf_len) == incl_index)
            ?__str : __str + suffix;
    }

    inline std::string assertEndsWith(const std::string&& suffix) const
    {
        return assertEndsWith(suffix);
    }

    std::string assertStartsWith (const std::string& prefix) const
    {
        size_t prf_len = prefix.length();
        size_t str_len = __str.length();
        size_t incl_index = __str.find(prefix);
        if (incl_index == std::string::npos) return prefix + __str;
        return
            (incl_index == 0)
            ? __str : prefix + __str;
    }
    std::string assertStartsWith (const std::string&& prefix)
    {
        return assertStartsWith(prefix);
    }

public:
    FixedString(const std::string& str){__str = str;}
    FixedString(const std::string&& str) { __str = str;}
    FixedString(const FixedString& fstr) {__str = fstr.str();}

    const std::string& str () const { return __str; }
};

class MoguNode : public FixedString
{
public:
    MoguNode(const std::string& node)
    {
        __str = node;
    }

    MoguNode(const std::string&& node)
    {
        __str = node;
    }

    MoguNode(const MoguNode& other)
    {
        __str = other.toString();
    }

    inline std::string toString() const
    {
        return __str;
    }

    inline std::string addPrefix(const std::string& prefix) const
    {
        std::string prf = prefix + ".";
        return assertStartsWith(prf);
    }
    inline std::string addPrefix(const std::string&& prefix) const
    {
        return addPrefix(prefix);
    }
    inline std::string addSuffix(const std::string& suffix)
    {
        std::string suf = "." + suffix;
        return assertEndsWith(suf);
    }
    inline std::string addSuffix(const std::string&& suffix) const
    {
        return assertEndsWith(suffix);
    }

};


#endif /* MOGUNODE_H_ */
