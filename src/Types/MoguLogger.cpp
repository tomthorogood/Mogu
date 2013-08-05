/*
 * MoguLogger.cpp
 *
 *  Created on: Aug 2, 2013
 *      Author: tom
 */

#include "MoguLogger.h"
#include "../Redis/DatabaseConfigReader.h"
#include "../Redis/NodeEditor.h"


MoguLogger::MoguLogger()
{
    if (override())
    {
        log_level = (int)LogLevel::ALL;
        return;
    }
 
    Application::loadDatabaseContexts();   
    assert(Application::contextMap);

    std::cout <<&prefixMap << std::endl;

    Redis::NodeEditor log_config (Prefix::meta, "log");
    if (!log_config.nodeExists())
    {
        log_level = (int) LogLevel::_NONE;
        return;
    }
    std::string cfg_level = log_config.read();
    log_level = (int) getLevelEnum(cfg_level);
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

