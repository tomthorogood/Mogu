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
        map[name] = value;
    }

    inline NodeValue retrieveSlot(const std::string& name)
    {
        NodeValue ret_val = map.at(name);
        map.erase(name);
        return ret_val;
    }

    inline NodeValue peekSlot(const std::string& name)
    {
        return map.at(name);
    }

private:
    std::unordered_map <std::string,NodeValue> map;

};


#endif /* SLOTMANAGER_H_ */
