#include <iostream>
#include <map>
#include <chrono>
#include <string>
#include "log_manager.h"
#include "log_message.h"
#include "date/date.h"

namespace LogXX
{
    
    std::map<levels, std::string> logLevelLables
    {
        {LOG_CRIT,      "CRITICAL"},
        {LOG_ERR,       "ERROR"},
        {LOG_WARNING,   "WARNING"},
        {LOG_INFO,      "INFO"},
        {LOG_DEBUG,     "DEBUG"}
    };
    
    
    message &message::format(const std::string &fmtStr)
    {
        m_format = boost::format(fmtStr);

        return *this;
    }

    void message::PostMessage()
    {
        manager::logMessage(shared_from_this());
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
        os << date << ' '
           << time << ' '
           << logLevelLables[msg->getLevel()] << ' '
           << '[' << msg->getThreadID() << "] "
           << msg->getFile().filename() << ':' << msg->getLine() << ' '
           << msg->getFunction() << ' '
           << msg->getMessage() << std::endl;
        
        return os;
    }
}