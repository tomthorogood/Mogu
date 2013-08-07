/*
 * Mogu_Logger.cpp
 *
 *  Created on: Aug 2, 2013
 *      Author: tom
 */

#include "Mogu_Logger.h"
#include "../Redis/DatabaseConfigReader.h"
#include "../Redis/NodeEditor.h"

const std::vector <std::pair <std::string,Log_Level>> log_level_map = {
    std::make_pair("ALL",Log_Level::all),
    std::make_pair("NOTICE", Log_Level::notice),
    std::make_pair("WARN",Log_Level::warn),
    std::make_pair("ERROR", Log_Level::error),
    std::make_pair("CRITICAL", Log_Level::critical),
    std::make_pair("NONE",Log_Level::none)
};

Mogu_Logger::MoguLogger()
{
    if (LOG_OVERRIDE)
    {
        log_level = (int)Log_Level::ALL;
        return;
    }
    Application::load_database_contexts(); 
    assert(Application::contextMap);

    Redis::Node_Editor log_config {Prefix::meta, "log"};
    if (!log_config.node_exists())
    {
        log_level = (int) Log_Level::none;
    }
    else
    {
        std::string s = log_config.read();
        log_level = (int) get_level_enum(s);
    }

    std::string cfg_level = log_config.read();
    log_level = (int) getLevelEnum(cfg_level);
}


std::string Mogu_Logger::get_level_name(Log_Level v) const
{
    for (std::pair <std::string, Log_Level> pr : log_level_map)
    {
        if (v==pr.second) return pr.first;
    }
    return "NONE";
}

Log_Level Mogu_Logger::get_level_enum(const std::string& s) const
{
    for (std::pair <std::string,Log_Level> pr : log_level_map)
    {
        if (s==pr.first) return pr.second;
    } 
    return Log_Level::none;
}
