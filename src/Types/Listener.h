/*
 * Listener.h
 *
 *  Created on: Oct 23, 2012
 *      Author: tom
 */

#ifndef LISTENER_H_
#define LISTENER_H_

#include <declarations.h>

class Listener
{

public:
    enum Type
    {
        NOT_SET, widget, string
    };
    Listener()
    {
        __init__();
    }

    Listener(
        Moldable* w)
    {
        __init__();
        __widget = w;
        type = widget;
    }

    Listener(
        const std::string& s)
    {
        __init__();
        __string = s;
        type = string;
    }

    inline Moldable& getWidget()
    {
        return *__widget;
    }
    inline std::string& getString()
    {
        return __string;
    }

    inline Type getType()
    {
        return type;
    }

private:
    Moldable* __widget;
    std::string __string;
    Type type;

    inline void __init__()
    {
        __widget = NULL;
        __string = EMPTY;
        type = NOT_SET;
    }
};

#endif /* LISTENER_H_ */
