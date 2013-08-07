/*
 * String_Map.h
 *
 *  Created on: Jun 17, 2013
 *      Author: tom
 */

#ifndef STRINGMAP_H_
#define STRINGMAP_H_

#include <map>
#include <string>

class String_Map {
public:
    String_Map(){}

    void erase_through(size_t first, size_t last);

    inline bool key_exists(size_t key) const 
        { return map.count(key); }

    inline std::string get (size_t key) const
    {
        return key_exists(key) ? 
            map.at(key) : ""
    }

    inline void set(size_t key, const std::string& val) 
        { map[key] = val; }

    inline void clear() { map.clear();}
    inline size_t size() const { return map.size(); }

private:
    std::map <size_t, std::string> map;

};



#endif /* STRINGMAP_H_ */
