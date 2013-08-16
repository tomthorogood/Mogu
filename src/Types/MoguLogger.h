/*
 * Mogu_Logger.h
 *
 *  Created on: Aug 2, 2013
 *      Author: tom
 */

#ifndef MOGULOGGER_H_
#define MOGULOGGER_H_

#include <vector>
#include <utility>
#include <string>
#include <iostream>

#ifdef DEBUG
#ifndef NOLOGOVERRIDE
    constexpr bool LOG_OVERRIDE = true;
#else
    constexpr bool LOG_OVERRIDE = false;
#endif
#else
    constexpr bool LOG_OVERRIDE = false;
#endif

enum class Log_Level
{
    all         =1  //Log all the things
    , notice    =2  //Log only notices
    , warn      =4  //Log only warnings
    , error     =8  //Log recoverable errors
    , critical  =16 //Log when things go real bad.
    , none      =32 //Do not log anything!
};




class Mogu_Logger
{
    int log_level;
    std::string get_level_name(Log_Level) const;
    Log_Level get_level_enum(const std::string&) const;

public:
    Mogu_Logger();

    Mogu_Logger(int i)
        : log_level(i)
    {
        if (LOG_OVERRIDE) log_level = (int) Log_Level::all;
    }

    inline bool do_log(Log_Level v) const
    {
        if (LOG_OVERRIDE) return true;
        if (log_level & (int) Log_Level::none) return false;
        if (log_level & (int) v) return true;
        if (log_level <= (int) v) return true;
        return false;
    }

    template <typename T, typename ... U>
    void log(Log_Level msg_level, T head, U... tail)
    {
        if (!do_log(msg_level)) return;
        if ((int) msg_level >= (int) Log_Level::warn)
        {
            vstderr(get_level_name(msg_level) + ": ");
            vstderr(head);
            vstderr(tail...);
            err_end();
        }
        else
        {
            vstdout(get_level_name(msg_level)+": ");
            vstdout(head);
            vstdout(tail...);
            out_end();
        }
    }

    void vstdout(){}
    void vstderr(){}

    template <typename T, typename ... U>
    void vstdout(T m, U... u)
    {
        std::cout << m;
        vstdout(u...);
    }

    template <typename T, typename ... U>
    void vstderr(T m, U... u)
    {
        std::cerr << m;
        vstderr(u...);
    }

    inline void out_end()
        { std::cout << std::endl;}

    inline void err_end()
        { std::cerr << std::endl;}

};

#endif /* MOGULOGGER_H_ */
