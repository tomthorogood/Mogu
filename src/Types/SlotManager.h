/*
 * Slot_Manager.h
 *
 *  Created on: Apr 22, 2013
 *      Author: tom
 */

#ifndef SLOTMANAGER_H_
#define SLOTMANAGER_H_
#include "Node_Value.h"
#include <unordered_map>
#include <string>

class Slot_Manager
{
public:
    Slot_Manager(){}

    inline void set_slot(const std::string& s, const Node_Value& v)
        { m[s]=v; }

    inline void get_slot(const std::string& s) const
        { return m.count(s) ? m.at(s) : Node_Value{""}; }

    inline void clear_slot(const std::string& s)
        { map.erase(s); }

private:
    std::unordered_map <std::string,Node_Value> m;
};


#endif /* SLOTMANAGER_H_ */
