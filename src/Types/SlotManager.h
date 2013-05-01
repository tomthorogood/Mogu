/*
 * SlotManager.h
 *
 *  Created on: Apr 22, 2013
 *      Author: tom
 */

#ifndef SLOTMANAGER_H_
#define SLOTMANAGER_H_
#include "NodeValue.h"
#include <unordered_map>
#include <string>
class SlotManager
{
public:
    SlotManager(){}

    inline void setSlot(const std::string& name, const NodeValue& value)
    {
        __map[name] = value;
    }

    inline NodeValue retrieveSlot(const std::string& name)
    {
        NodeValue ret_val = __map.at(name);
        __map.erase(name);
        return ret_val;
    }

    inline NodeValue peekSlot(const std::string& name)
    {
        return __map.at(name);
    }

private:
    std::unordered_map <std::string,NodeValue> __map;

};


#endif /* SLOTMANAGER_H_ */
