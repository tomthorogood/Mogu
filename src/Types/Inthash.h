#ifndef INTHASH_H_
#define INTHASH_H_


template <typename T>
struct Int_Hash 
{
public:
    Int_Hash() {}
    Int_Hash(const T& t) {}
    size_t operator()(T t)const noexcept
    {
        return static_cast<size_t>(t);
    }
};

#endif 
