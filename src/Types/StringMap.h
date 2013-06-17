/*
 * StringMap.h
 *
 *  Created on: Jun 17, 2013
 *      Author: tom
 */

#ifndef STRINGMAP_H_
#define STRINGMAP_H_

#include <declarations.h>
#include <stdexcept>

class StringMap {
public:
    StringMap(){}

    inline void erase_through(size_t first, size_t last) {
        for (size_t i = first; i <= last; ++i) {
            if (key_exists(i))
                __map.erase(i);
        }
    }

    inline bool key_exists(size_t key) const {
        try {
            __map.at(key);
        }
        catch (const std::out_of_range&) {
            return false;
        }
        return true;
    }

    inline std::string get (size_t key) const {
        if (key_exists(key)) return __map.at(key);
        else return EMPTY;
    }

    inline void set(size_t key, const std::string& val) {
        __map[key] = val;
    }

    inline void clear() { __map.clear();}
    inline size_t size() const { return __map.size(); }

private:
    std::unordered_map <size_t, std::string> __map;

};



#endif /* STRINGMAP_H_ */
