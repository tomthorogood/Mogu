/*
 * inline_utils.h
 *
 *  Created on: Sep 10, 2012
 *      Author: tom
 */

#ifndef INLINE_UTILS_H_
#define INLINE_UTILS_H_

#include <sstream>

inline std::string itoa(
    int i)
{
    std::stringstream s;
    s << i;
    return s.str();
}

inline std::string ftoa(
    double d)
{
    std::stringstream s;
    s << d;
    return s.str();
}

template<typename T, typename U>
class create_map
{
private:
    std::map<T, U> m_map;
public:
    create_map(
        const T& key, const U& val)
    {
        m_map[key] = val;
    }

    create_map<T, U>& operator()(
        const T& key, const U& val)
    {
        m_map[key] = val;
        return *this;
    }

    operator std::map<T, U>()
    {
        return m_map;
    }
};
#endif /* INLINE_UTILS_H_ */
