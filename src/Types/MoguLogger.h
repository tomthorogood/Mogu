/*
 * MoguLogger.h
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

enum class LogLevel
{
    ALL         =1  //Log all the things
    , NOTICE    =2  //Log only notices
    , WARN      =4  //Log only warnings
    , ERROR     =8  //Log recoverable errors
    , CRITICAL  =16 //Log when things go real bad.
    , _NONE      =32 //Do not log anything!
};

const std::vector <std::pair <std::string,LogLevel>> logLevelV = {
    std::make_pair("ALL",LogLevel::ALL),
    std::make_pair("NOTICE", LogLevel::NOTICE),
    std::make_pair("WARN",LogLevel::WARN),
    std::make_pair("ERROR", LogLevel::ERROR),
    std::make_pair("CRITICAL", LogLevel::CRITICAL),
    std::make_pair("NONE",LogLevel::_NONE)
};


#ifdef DEBUG
#ifndef NOLOGOVERRIDE
    static bool LOG_OVERRIDE = true;
#else
    static bool LOG_OVERRIDE = false;
#endif
#else
    static bool LOG_OVERRIDE = false;
#endif

class MoguLogger
{
    int log_level;
    std::string getLevelName(LogLevel) const;
    LogLevel getLevelEnum(const std::string&) const;
public:

    MoguLogger();

    MoguLogger(int logLevel_)
        :log_level(logLevel_)
    { if (override()) log_level = (int) LogLevel::ALL; }



    const bool override() const { return LOG_OVERRIDE; }

    inline bool doLog(LogLevel lv) const
    {
        // When in debug mode, log everything.
        if (LOG_OVERRIDE) return true;
        // Do not log anything if this bit has been set.
        if (log_level & (int) LogLevel::_NONE) return false;

        // If it's an exact match, log it.
        if (log_level & (int) lv) return true;

        // If the default log level is more verbose than the
        // the one passed in, log it.
        if ((int) log_level <= (int) lv) return true;

        // Otherwise, do not.
        return false;
    }

    template <typename T, typename ... U>
    void log(LogLevel msg_level, T head, U... tail)
    {
        if ((int)msg_level >= (int) LogLevel::WARN)
        {
            vstderr(getLevelName(msg_level)+": ");
            vstderr(head);
            vstderr(tail...);
            err_end();
        }
        else
        {
            vstdout(getLevelName(msg_level)+": ");
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

namespace Application {
    static MoguLogger log;
}

#endif /* MOGULOGGER_H_ */
