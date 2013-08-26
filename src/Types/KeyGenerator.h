#ifndef KEY_GENERATOR_H_
#define KEY_GENERATOR_H_

#include "Generator.h"


template <class T>
class Key_Generator : public Generator<T>
{
public:
    template<class U>
    Key_Generator (const std::unordered_map<T,U>& m)
    {
        for (auto iter : m)
        {
            Generator<T>::push(iter.first);
        }
    }
};

#endif //KEY_GENERATOR_H_
