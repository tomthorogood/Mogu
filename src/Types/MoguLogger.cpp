/*
 * MoguLogger.cpp
 *
 *  Created on: Aug 2, 2013
 *      Author: tom
 */

#include "MoguLogger.h"
#include <declarations.h>
#include <Redis/NodeEditor.h>


MoguLogger::MoguLogger()
{
    if (override())
    {
        log_level = (int)LogLevel::ALL;
        return;
    }

    Redis::NodeEditor log_config (Prefix::meta, "log");
    if (!log_config.nodeExists())
    {
        log_level = (int) LogLevel::_NONE;
        return;
    }
    std::vector <std::string> levels;
    log_config.read(levels);

    for (std::string level : levels)
    {
        log_level |= (int) getLevelEnum(level);
    }
}

std::string MoguLogger::getLevelName(LogLevel l) const
{
    for (std::pair<std::string,LogLevel> pr : logLevelV)
    {
        if (l==pr.second)
            return pr.first;
    }
    return "NONE";
}

LogLevel MoguLogger::getLevelEnum(const std::string& s) const
{
    for (std::pair<std::string,LogLevel> pr : logLevelV)
    {
        if (s==pr.first)
            return pr.second;
    }
    return LogLevel::_NONE;
}

