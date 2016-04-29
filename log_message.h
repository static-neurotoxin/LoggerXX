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

#include "log_hash.h"

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
        message()
            : m_line(0)
            , m_level(LOG_DEBUG)
            , m_logTime(std::chrono::system_clock::now())
            , m_threadID(std::this_thread::get_id())
        {
        }

        message(const boost::filesystem::path &file, const std::string &function, uint32_t line, levels level) = delete;
        message(const message &) = delete;
        message(const message &&) = delete;

        message *set_file(const boost::filesystem::path &path)
        {
            m_file = path;
            return this;
        }

        message *set_function(const std::string &function)
        {
            m_function = function;
            return this;
        }

        message *set_line(uint32_t line)
        {
            m_line = line;
            return this;
        }

        message *set_level(levels level)
        {
            m_level = level;
            return this;
        }

        message *set_hash(uint64_t hash)
        {
            m_hash = hash;

            return this;
        }

        message *set_extendedFunction(const std::string &function)
        {
            m_extendedFunction = function;
            return this;
        }

        message *set_class(const std::string &class)
        {
            m_class = class;
            return this;
        }

        message *set_module(const std::string &module)
        {
            m_module = module;
            return this;
        }

        // template <typename... Args>
        // static void LogMessage(const std::string &file, const std::string &function, uint32_t line, levels level, const std::string &format, const Args&... args)
        // {
        //     auto msg(std::make_shared<message>(file, function, line, level));
        //     msg->format(format).print(args...);
        //     msg->PostMessage();
        // }

        message *PostMessage();

        template <typename... Args>
        message *format(const std::string &fmtStr, const Args&... args)
        {
            m_format = boost::format(fmtStr);
            print(args...);

            return this;
        }

        message *print()
        {
            return this;
        }

        template <typename T>
        message *print(const T& t)
        {
            m_format % t;
            return this;
        }

        message *print(bool b)
        {
            m_format % (b ? "true" : "false");
            return this;
        }

        template <typename First, typename... Rest>
        message *print(const First& first, const Rest&... rest)
        {
            print(first);
            print(rest...);
            return this;
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
        std::string                             m_extendedFunction;
        std::string                             m_class;
        std::string                             m_module;
        uint32_t                				m_line;
        uint64_t                                m_hash;
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

#define LOG_CLASS  ""
#define LOG_MODULE ""

#define LOG_CLASS_NAME(x)   \
#ifdef LOG_CLASS            \
#undef LOG_CLASS            \
#endif                      \
#define LOG_CLASS x

#define LOG_MODULE_NAME(x)   \
#ifdef LOG_MODULE            \
#undef LOG_MODULE            \
#endif                       \
#define LOG_MODULE x

#define _log(LOG_LEVEL, ...)                \
    std::make_shared<message>()->           \
        set_level(LOG_LEVEL)->              \
        set_file(__FILE__)->                \
        set_function(__FUNCTION__)->        \
        set_line(__LINE__)->                \
        set_extendedFunction(FUNC_NAME)->   \
        set_hash(LINECRC)->                 \
        set_class(LOG_CLASS)->              \
        set_module(LOG_MODULE)->            \
        format(__VA_ARGS__)->               \
        PostMessage();

#define _trace(...)	_log(LogXX::LOG_DEBUG,   __VA_ARGS__);
#define _info(...)	_log(LogXX::LOG_INFO,    __VA_ARGS__);
#define _warn(...)	_log(LogXX::LOG_WARNING, __VA_ARGS__);
#define _err(...)	_log(LogXX::LOG_ERR,     __VA_ARGS__);
#define _sev(...)	_log(LogXX::LOG_CRIT,    __VA_ARGS__);

#endif//_LOG_H_