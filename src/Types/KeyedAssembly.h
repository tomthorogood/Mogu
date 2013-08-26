#ifndef WIDGET_ASSEMBLY_H_
#define WIDGET_ASSEMBLY_H_

#include "WidgetAssembly.h"

template <class T> 
class Keyed_Assembly
{
public:
    Keyed_Assembly () {}
    Keyed_Assembly (const T& v) : key(v) {}
    Keyed_Assembly (const T& v, Widget_Assembly& w) : key(v), a(w) {}
    ~Keyed_Assembly () {}
    inline Widget_Assembly& get_assembly () { return a; }
    
    inline void set_key(const T& k) { key = k; }
    inline const T& get_key() const { return key; }
    inline void set_assembly(const Widget_Assembly& w) { a=w; }

    inline bool operator< (const Keyed_Assembly& o) const
        { return key < o.get_key(); }
    
    inline bool operator> (const Keyed_Assembly& o) const 
        { return key > o.get_key(); }
    
    inline bool operator>= (const Keyed_Assembly& o) const
        { return key >= o.get_key(); }
    
    inline bool operator<= (const Keyed_Assembly& o) const
        { return key <= o.get_key(); }
    
    inline bool operator== (const Keyed_Assembly& o) const
        { return key == o.get_key(); }

private:
    T key {};
    Widget_Assembly a {};
};

#endif //WIDGET_ASSEMBLY_H_
