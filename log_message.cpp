#include <iostream>
#include <map>
#include <chrono>
#include <string>
#include "log.h"
#include "logger.h"
#include "date.h"

std::map<logLevels, std::string> logLevelLables
{
    {LOG_EMERG,     "EMERGENCY"},
    {LOG_ALERT,     "ALERT"},
    {LOG_CRIT,      "CRITICAL"},
    {LOG_ERR,       "ERROR"},
    {LOG_WARNING,   "WARNING"},
    {LOG_NOTICE,    "NOTICE"},
    {LOG_INFO,      "INFO"},
    {LOG_DEBUG,     "DEBUG"},
    {LOG_OFF,       "---"}
    };

log &log::format(const std::string &fmtStr)
{
    m_format = boost::format(fmtStr);

    return *this;
}

void log::PostMessage()
{
    logger::logMessage(shared_from_this());
}

std::string log::getMessage() const
{
    std::stringstream s;
    s << m_format;
    return s.str();
}

std::ostream &operator <<(std::ostream &os, const std::shared_ptr<log> msg)
{
    auto log_time_point(msg->getDate());
    auto log_date(date::floor<date::days>(log_time_point));
    auto date = date::year_month_day{log_date};
    auto time(date::make_time(log_time_point - log_date));
    os << date << ' '
       << time << ' '
       << logLevelLables[msg->getLevel()] << ' '
       << '[' << msg->getThreadID() << "] "
       << msg->getFile() << ':' << msg->getLine() << ' '
       << msg->getFunction() << ' '
       << msg->getMessage() << std::endl;
    
    return os;
}