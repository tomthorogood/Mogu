/*
 * MoguNode.h
 *
 *  Created on: Dec 9, 2012
 *      Author: tom
 */

#ifndef MOGUNODE_H_
#define MOGUNODE_H_

class MoguNode
{
    std::string __str;
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
        return prefix + "." + __str;
    }
    std::string addPrefix(const std::string&& prefix) const
    {
        return prefix + "." + __str;
    }
    std::string addSuffix(const std::string& suffix)
    {
        return __str + "." + suffix;
    }
    std::string addSuffix(const std::string&& suffix) const
    {
        return __str + "." + suffix;
    }

};


#endif /* MOGUNODE_H_ */
