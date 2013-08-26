#ifndef NODE_H_
#define NODE_H_

template <class T> 
class Node
{
public:
    Node(){}
    Node(const T& t) : v(t) {}
    inline T get_val() {return v;}
    inline void set_val(cnst T& t) { v = t;}

private:
    T v {};
}

#endif
