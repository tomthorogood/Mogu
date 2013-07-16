#include <map>

/* An implementation of a std::map
 * that allows for const instances as well as 
 * the index operator ([]) by using C++11 initializer
 * lists.
 *
 * const static LockedMap <int, std::string> foo = {
 *      {   1   , "foo" },
 *      {   2   , "bar" },
 *      {   3   , "baz" }
 * };
 *
 */
template <class T, class U>
class LockedMap
{
    std::map <T,U> __map;
public:
    LockedMap(std::initializer_list <typename std::map<T,U>::value_type> lst)
        : __map(lst){}
    const U& operator[] (const T& key) const
    {
        return __map.at(key);
    }
} ;
