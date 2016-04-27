// LoggerPOC.cpp : Defines the entry point for the console application.
//
#pragma once
#ifndef _LOG_H_
#define _LOG_H_

#include <iostream>
#include <string>
#include <sstream>
#include <cstdint>
#include <chrono>
#include <memory>
#include <mutex>
#include <system_error>
#include <queue>
#include <thread>
#include <condition_variable>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include "date/date.h"

enum logLevels
{
    LOG_EMERG   = 0,       /* system is unusable */
    LOG_ALERT   = 1,       /* action must be taken immediately */
    LOG_CRIT    = 2,       /* critical conditions */
    LOG_ERR     = 3,       /* error conditions */
    LOG_WARNING = 4,       /* warning conditions */
    LOG_NOTICE  = 5,       /* normal but significant condition */
    LOG_INFO    = 6,       /* informational */
    LOG_DEBUG   = 7,       /* debug-level messages */
    LOG_OFF     = 0xFF    // special case used to set the default threshold not for Log() calls
};

class log : public std::enable_shared_from_this<log>
{
public:
    log(const std::string &file, const std::string &function, uint32_t line, logLevels level)
    : m_file(file)
    , m_function(function)
    , m_line(line)
    , m_level(level)
    , m_logTime(std::chrono::system_clock::now())
    , m_threadID(std::this_thread::get_id())
    {
    }

    log() = delete;
    log(const log &) = delete;
    log(const log &&) = delete;

    template <typename... Args>
    static void LogMessage(const std::string &file, const std::string &function, uint32_t line, logLevels level, const std::string &format, const Args&... args)
    {
        auto msg(std::make_shared<log>(file, function, line, level));
        msg->format(format).print(args...);
        msg->PostMessage();
    }
    
    void PostMessage();
    

    log &format(const std::string &fmtStr);

    void print(){}

    template <typename T>
    void print(const T& t)
    {
        m_format % t;
    }

    void print(bool b)
    {
        m_format % (b ? "true" : "false");
    }

    template <typename First, typename... Rest>
    void print(const First& first, const Rest&... rest)
    {
        print(first);
        print(rest...);
    }

    std::string getMessage() const;
    uint32_t    getLine()     const {return m_line;}
    logLevels   getLevel()    const {return m_level;}
    
    const std::string             				&getFile()     const {return m_file;}
    const std::string             				&getFunction() const {return m_function;}
    const std::chrono::system_clock::time_point &getDate()     const {return m_logTime;}
    const std::thread::id         				&getThreadID() const {return m_threadID;}

private:
    boost::format           				m_format;
    std::string             				m_file;
    std::string             				m_function;
    uint32_t                				m_line;
    logLevels               				m_level;
    std::chrono::system_clock::time_point 	m_logTime;
    std::thread::id         				m_threadID;
};

std::ostream &operator <<(std::ostream &os, const std::shared_ptr<log> msg);

#define _trace0(string)     log::LogMessage(__FILE__, __func__, __LINE__, LOG_DEBUG,   "%1%", string);
#define _info0(string)      log::LogMessage(__FILE__, __func__, __LINE__, LOG_INFO,    "%1%", string);
#define _warn0(string)      log::LogMessage(__FILE__, __func__, __LINE__, LOG_WARNING, "%1%", string);
#define _err0(string)       log::LogMessage(__FILE__, __func__, __LINE__, LOG_ERR,     "%1%", string);
#define _sev0(string)       log::LogMessage(__FILE__, __func__, __LINE__, LOG_CRIT,    "%1%", string);
#define _fat0(string)       log::LogMessage(__FILE__, __func__, __LINE__, LOG_ALERT,   "%1%", string);

#define _trace(format,...)	log::LogMessage(__FILE__, __func__, __LINE__, LOG_DEBUG,   format, __VA_ARGS__);
#define _info(format,...)	log::LogMessage(__FILE__, __func__, __LINE__, LOG_INFO,    format, __VA_ARGS__);
#define _warn(format,...)	log::LogMessage(__FILE__, __func__, __LINE__, LOG_WARNING, format, __VA_ARGS__);
#define _err(format,...)	log::LogMessage(__FILE__, __func__, __LINE__, LOG_ERR,     format, __VA_ARGS__);
#define _sev(format,...)	log::LogMessage(__FILE__, __func__, __LINE__, LOG_CRIT,    format, __VA_ARGS__);
#define _fat(format,...)	log::LogMessage(__FILE__, __func__, __LINE__, LOG_ALERT,   format, __VA_ARGS__);

#endif//_LOG_H_