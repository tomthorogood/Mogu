#ifndef COMPARABLE_H_
#define COMPARABLE_H_

template <class T, class U=T>
struct Comparable {
    inline virtual bool operator() (const T& a, const U& b) { return a < b; }
};

#endif
