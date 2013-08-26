#ifndef GENERATOR_H_
#define GENERATOR_H_

template <class T>
class Generator 
{
public:
    Generator(){}
    Generator(const T& q_) : q(q_) {}
    virtual ~Generator(){}

    inline virtual void push(T v) {q.push(v);}
    
    inline virtual T yield () {
        T v = q.front();
        q.pop();
        return v;
    }
    
    inline virtual void clear() { while (q.size()) q.pop(); }

    inline virtual operator bool () { return static_cast<bool>(q.size()); }

private:
    std::queue <T> q{};
};

#endif
