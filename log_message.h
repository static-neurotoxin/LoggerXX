/**
 * @file   log_message.h
 * @author Gordon "Lee" Morgan (valkerie.fodder@gmail.com)
 * @copyright Copyright © Gordon "Lee" Morgan May 2016. This project is released under the MIT License
 * @date   May 2016
 * @brief  log message container.
 * @details A class to encapsulate and format a "printf" style debug logging messages and associated macros
 */

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
    //! Default log levels
    enum levels
    {
        LOG_CRIT,    //!< critical conditions
        LOG_ERR,     //!< error conditions
        LOG_WARNING, //!< warning conditions
        LOG_INFO,    //!< informational
        LOG_DEBUG,   //!< debug level messages
        LOG_NONE     //!< no logging specified
    };

    //! Container class for log message
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

            //! Set the filename of the source file
            message *set_file(const boost::filesystem::path &path)
            {
                m_file = path;
                return this;
            }

            //! Set the function name
            message *set_function(const std::string &function)
            {
                m_function = function;
                return this;
            }

            //! Set the line number
            message *set_line(uint32_t line)
            {
                m_line = line;
                return this;
            }

            //! Set the log level
            message *set_level(levels level)
            {
                m_level = level;
                return this;
            }

            //! Set a unique hash for the log message
            message *set_hash(uint64_t hash)
            {
                m_hash = hash;

                return this;
            }

            //! Set a more descriptive function name
            message *set_extendedFunction(const std::string &function)
            {
                m_extendedFunction = function;
                return this;
            }

            //! Associate message with a class
            message *set_class(const std::string &className)
            {
                m_class = className;
                return this;
            }

            //! Associate message with an arbitrary module
            message *set_module(const std::string &module)
            {
                m_module = module;
                return this;
            }

            //! Queue message with log manager
            message *PostMessage();

            //! Format log message
            //  @param[in] fmtStr a printf style format string (see boost::format for details)
            //  @param[in] args   arguments to log
            template <typename... Args>
            message *format(const std::string &fmtStr, const Args &... args)
            {
                m_format = boost::format(fmtStr);
                print(args...);

                return this;
            }

            //! print terminator
            message *print()
            {
                return this;
            }

            //! typesafe print for any argument that overrides ostream << operator
            template <typename T>
            message *print(const T &t)
            {
                m_format % t;
                return this;
            }

            //! Example print function to override type (bool)
            message *print(bool b)
            {
                m_format % (b ? "true" : "false");
                return this;
            }

            //! Recursive print function
            template <typename First, typename... Rest>
            message *print(const First &first, const Rest &... rest)
            {
                print(first);
                print(rest...);
                return this;
            }


            // *INDENT-OFF*
            std::string getMessage() const;                     //!< Get formatted log message
            uint32_t    getLine()     const { return m_line; }  //!< Get log message line number
            levels      getLevel()    const { return m_level; } //!< Get log message log level

            const boost::filesystem::path               &getFile()     const { return m_file; }     //!< Get log message file
            const std::string                           &getFunction() const { return m_function; } //!< Get log message function
            const std::chrono::system_clock::time_point &getDate()     const { return m_logTime; }  //!< Get log message timestamp
            const std::thread::id                       &getThreadID() const { return m_threadID; } //!< Get log message thread ID
            uint64_t                                     getHash()     const { return m_hash; }     //!< Get log message hash
            // *INDENT-ON*

        private:
            boost::format                           m_format;
            boost::filesystem::path                 m_file;
            std::string                             m_function;
            std::string                             m_extendedFunction;
            std::string                             m_class;
            std::string                             m_module;
            uint32_t                                m_line;
            uint64_t                                m_hash;
            levels                                  m_level;
            std::chrono::system_clock::time_point   m_logTime;
            std::thread::id                         m_threadID;
    };

    //! Put log message on to std::ostream
    std::ostream &operator <<(std::ostream &os, const std::shared_ptr<message> msg);
}

#ifdef _MSC_VER // Visual Studio
#define FUNC_NAME __FUNCSIG__
#else //clang and gcc
#define FUNC_NAME __PRETTY_FUNCTION__
#endif

// There is no standard macro for the current class
#ifndef LOG_CLASS
#define LOG_CLASS  ""
#endif


#ifndef LOG_MODULE
#define LOG_MODULE ""
#endif

//! Example/sample log macro
#define _log(LOG_LEVEL, ...)                \
    std::make_shared<LogXX::message>()->    \
        set_level(LOG_LEVEL)->              \
        set_file(__FILE__)->                \
        set_function(__func__)->            \
        set_line(__LINE__)->                \
        set_extendedFunction(FUNC_NAME)->   \
        set_hash(LINECRC)->                 \
        set_class(LOG_CLASS)->              \
        set_module(LOG_MODULE)->            \
        format(__VA_ARGS__)->               \
        PostMessage();

#define _trace(...) _log(LogXX::LOG_DEBUG,   __VA_ARGS__); //!< Log macro trace level
#define _info(...)  _log(LogXX::LOG_INFO,    __VA_ARGS__); //!< Log macro info level
#define _warn(...)  _log(LogXX::LOG_WARNING, __VA_ARGS__); //!< Log macro warning level
#define _err(...)   _log(LogXX::LOG_ERR,     __VA_ARGS__); //!< Log macro error level
#define _sev(...)   _log(LogXX::LOG_CRIT,    __VA_ARGS__); //!< Log macro critical level

#endif//_LOG_H_