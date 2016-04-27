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

namespace LogXX
{
    enum levels
    {
        LOG_CRIT,    /* critical conditions */
        LOG_ERR,     /* error conditions */
        LOG_WARNING, /* warning conditions */
        LOG_INFO,    /* informational */
        LOG_DEBUG,   /* debug-level messages */
    };

    class message : public std::enable_shared_from_this<message>
    {
    public:
        message(const boost::filesystem::path &file, const std::string &function, uint32_t line, levels level)
        : m_file(file)
        , m_function(function)
        , m_line(line)
        , m_level(level)
        , m_logTime(std::chrono::system_clock::now())
        , m_threadID(std::this_thread::get_id())
        {
        }

        message() = delete;
        message(const message &) = delete;
        message(const message &&) = delete;

        template <typename... Args>
        static void LogMessage(const std::string &file, const std::string &function, uint32_t line, levels level, const std::string &format, const Args&... args)
        {
            auto msg(std::make_shared<message>(file, function, line, level));
            msg->format(format).print(args...);
            msg->PostMessage();
        }
        
        void PostMessage();
        

        message &format(const std::string &fmtStr);

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
        levels      getLevel()    const {return m_level;}
        
        const boost::filesystem::path             	&getFile()     const {return m_file;}
        const std::string             				&getFunction() const {return m_function;}
        const std::chrono::system_clock::time_point &getDate()     const {return m_logTime;}
        const std::thread::id         				&getThreadID() const {return m_threadID;}

    private:
        boost::format           				m_format;
        boost::filesystem::path             	m_file;
        std::string             				m_function;
        uint32_t                				m_line;
        levels               				    m_level;
        std::chrono::system_clock::time_point 	m_logTime;
        std::thread::id         				m_threadID;
    };

    std::ostream &operator <<(std::ostream &os, const std::shared_ptr<message> msg);
}

#ifdef _MSC_VER // Visual Studio
    #define FUNC_NAME __FUNCSIG__
#else //clang and gcc
    #define FUNC_NAME __PRETTY_FUNCTION__
#endif

#define _trace0(string)     LogXX::message::LogMessage(__FILE__, FUNC_NAME, __LINE__, LogXX::LOG_DEBUG,   "%1%", string);
#define _info0(string)      LogXX::message::LogMessage(__FILE__, FUNC_NAME, __LINE__, LogXX::LOG_INFO,    "%1%", string);
#define _warn0(string)      LogXX::message::LogMessage(__FILE__, FUNC_NAME, __LINE__, LogXX::LOG_WARNING, "%1%", string);
#define _err0(string)       LogXX::message::LogMessage(__FILE__, FUNC_NAME, __LINE__, LogXX::LOG_ERR,     "%1%", string);
#define _sev0(string)       LogXX::message::LogMessage(__FILE__, FUNC_NAME, __LINE__, LogXX::LOG_CRIT,    "%1%", string);

#define _trace(format,...)	LogXX::message::LogMessage(__FILE__, FUNC_NAME, __LINE__, LogXX::LOG_DEBUG,   format, __VA_ARGS__);
#define _info(format,...)	LogXX::message::LogMessage(__FILE__, FUNC_NAME, __LINE__, LogXX::LOG_INFO,    format, __VA_ARGS__);
#define _warn(format,...)	LogXX::message::LogMessage(__FILE__, FUNC_NAME, __LINE__, LogXX::LOG_WARNING, format, __VA_ARGS__);
#define _err(format,...)	LogXX::message::LogMessage(__FILE__, FUNC_NAME, __LINE__, LogXX::LOG_ERR,     format, __VA_ARGS__);
#define _sev(format,...)	LogXX::message::LogMessage(__FILE__, FUNC_NAME, __LINE__, LogXX::LOG_CRIT,    format, __VA_ARGS__);

#endif//_LOG_H_