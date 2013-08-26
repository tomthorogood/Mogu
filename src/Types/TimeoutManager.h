#ifndef TIMEOUT_MANAGER_H_
#define TIMEOUT_MANAGER_H_

#include <time.h>

class Timeout_Manager
{
public:
    Timeout_Manager(){}
    Timeout_Manager(const time_t& t) : timeout_secs(t) {}

    inline void set_timeout(const time_t& t) { timeout_secs = t; }

    inline bool expired()
    {
        time_t t {};
        time(&t);
        bool e {false};
        if ( (t - last_call) > timeout_secs)
        {
            e = true;
            last_call = t;
        }
        return e;
    }
private:
    time_t timeout_secs {};
    time_t last_call {};
};

#endif
