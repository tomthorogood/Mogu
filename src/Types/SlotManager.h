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
#include <Types/MoguLogger.h>

class SlotManager
{
public:
    SlotManager(){}

    inline void setSlot(const std::string& name, const NodeValue& value)
    {
        if (name.empty())
        {
            Application::log.log(LogLevel::CRITICAL,
                "Attempting to set an anonymous slot. Something has "
                , "to have gone wrong for this to be the case.");
            return ;
        }
        if (value.getString().empty())
        {
            Application::log.log(LogLevel::WARN, "Setting an empty string "
                , "(", value.getString(), ") "
                , "in slot ", name, ". Refusing to comply.");
            return;
        }
        map[name] = value;
    }

    inline NodeValue retrieveSlot(const std::string& name) const
    {
        if (!map.count(name)) return NodeValue("");
        return NodeValue(map.at(name));
    }

    inline void clearSlot(const std::string& name)
    { map.erase(name);}

private:
    std::unordered_map <std::string,NodeValue> map;

};


#endif /* SLOTMANAGER_H_ */
