/**
 * @file   log_message.cpp
 * @author Gordon "Lee" Morgan (valk.erie.fod.der+logxx@gmail.com)
 * @copyright Copyright © Gordon "Lee" Morgan May 2016. This project is released under the [MIT License](license.md)
 * @date   May 2016
 * @brief  log message container.
 * @details A class to encapsulate and format a "printf" style debug logging messages and associated macros
 */

#include <iostream>
#include <map>
#include <chrono>
#include <string>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include "log_manager.h"
#include "log_message.h"
#include "date/date.h"

namespace LogXX
{
    //! Map log levels to human readable strings
    //! \note this multimap is used for formatting the log messages and for reading the configuration file
    std::multimap<levels, std::string> logLevelLables
    {
        {LOG_NONE,      "NONE"},
        {LOG_NONE,      "OFF"},
        {LOG_CRIT,      "CRITICAL"},
        {LOG_ERR,       "ERROR"},
        {LOG_WARNING,   "WARNING"},
        {LOG_INFO,      "INFO"},
        {LOG_DEBUG,     "DEBUG"},
        {LOG_ALL,       "ON"},
        {LOG_ALL,       "ALL"}
    };

    //! Helper to convert text to level
    levels message::stringToLevel(const std::string &levelStr)
    {
        levels level(LOG_NONE);

        auto result(std::find_if(logLevelLables.begin(), logLevelLables.end(), [&levelStr](const auto & i) -> bool
        {
            return boost::algorithm::iequals(levelStr, i.second);
        }));

        if(result != logLevelLables.end())
        {
            level = result->first;
        }

        return level;
    }

    message *message::PostMessage()
    {
        manager::logMessage(shared_from_this());

        return this;
    }

    std::string message::getMessage() const
    {
        std::stringstream s;
        s << m_format;
        return s.str();
    }

    std::ostream &operator <<(std::ostream &os, const std::shared_ptr<message> msg)
    {
        auto log_time_point(msg->getDate());
        auto log_date(date::floor<date::days>(log_time_point));
        auto date = date::year_month_day{log_date};
        auto time(date::make_time(log_time_point - log_date));
        std::string levelText;
        auto levelIterator(logLevelLables.find(msg->getLevel()));

        if(levelIterator != logLevelLables.end())
        {
            levelText = levelIterator->second;
        }
        else
        {
            levelText = "level:" + std::to_string(msg->getLevel());
        }

        os << date << ' '
           << time << ' '
           << levelText << ' '
           << '[' << msg->getThreadID() << "] "
           << msg->getFile().filename().generic_string() << ':' << msg->getLine() << ' '
           << msg->getFunction() << ' '
           << msg->getMessage() << std::endl;

        return os;
    }
}