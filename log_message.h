/**
 * @file   log_message.h
 * @author Gordon "Lee" Morgan (valk.erie.fod.der+logxx@gmail.com)
 * @copyright Copyright © Gordon "Lee" Morgan May 2016. This project is released under the [MIT License](license.md)
 * @date   May 2016
 * @brief  log message container.
 * @details A class to encapsulate and format a "printf" style debug logging messages and associated macros
 *
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
#include <unordered_map>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include "date/date.h"

#include "log_hash.h"
#include "log_format.h"

namespace LogXX
{
    //! Default log levels
    enum levels
    {
        LOG_NONE,    //!< turn off all logging
        LOG_CRIT,    //!< critical conditions
        LOG_ERR,     //!< error conditions
        LOG_WARNING, //!< warning conditions
        LOG_INFO,    //!< informational
        LOG_DEBUG,   //!< debug level messages
        LOG_ALL      //!< turn on all logging
    };

    //! Container class for log message
    class message : public std::enable_shared_from_this<message>
    {
        public:
            message(const std::string &defaultFormatString = "%1% %2% %3% [%4%] %5% %7%")
                : m_line(0)
                , m_level(LOG_DEBUG)
                , m_logTime(std::chrono::system_clock::now())
                , m_threadID(std::this_thread::get_id())
                , m_defaultFormatString(defaultFormatString)
            {
            }

            message(const boost::filesystem::path &file, const std::string &function, uint32_t line, levels level) = delete;
            message(const message &) = delete;
            message(const message &&) = delete;

            /** @name Message construction
            *  These are a collection of functions to be used to construct a debug mesage, typically from within a macro
            */
            ///@{
            /**
             * Set the filename of the source file
             * @param[in] path filename that log message occurs in
             * @note Typically this function is called with the ```__FILE__``` macro
             */
            message *set_file(const boost::filesystem::path &path)
            {
                m_file = path;
                return this;
            }

            /**
              * Set the function name
              * @param[in] func function name
              * @note Typically this function is called with the  ``__func__`` macro
              */
            message *set_function(const std::string &func)
            {
                m_function = func;
                return this;
            }

            /**
             * Set the line number
              * @param[in] line line number
              * @note Typically this function is called with the  `__LINE__` macro
             */
            message *set_line(uint32_t line)
            {
                m_line = line;
                return this;
            }

            //! Set the log level
            //! \param[in] level Set the log level
            message *set_level(levels level)
            {
                m_level = level;
                return this;
            }

            //! Set a unique hash for the log message
            //! \param[in] hash a unique hash used to identify a log message location
            //! \note the provided `LINECRC` macro is provieded to generate a CRC64 based on `__FILE__`, `__LINE__ and `__func__` at compile time
            message *set_hash(uint64_t hash)
            {
                m_hash = hash;

                return this;
            }

            //! Set a more descriptive function name
            //! \param[in] func a more descriptive function macro
            //! \note used to wrap the compiler specific function name macros like GCCs `__PRETTY_FUNCTION__` or MSVCs `__FUNCSIG__`
            message *set_extendedFunction(const std::string &func)
            {
                m_extendedFunction = func;
                return this;
            }

            //! Associate message with a class
            //! \param[in] className name of the class associated with the log message
            //! \note This must be managed manually, there is no generic way of accessing class name data, _afaik_.
            message *set_class(const std::string &className)
            {
                m_class = className;
                return this;
            }

            //! Associate message with an arbitrary module
            //! \param[in] module name of the module associated with the log message
            //! \note This is intended to allow log messages to be associated with arbitrary groupings
            message *set_module(const std::string &module)
            {
                m_module = module;
                return this;
            }

            //! Set the default header format string
            message *set_header_format_string(const std::string &formatString)
            {
                m_defaultFormatString = formatString;
                return this;
            }
            ///@}

            //! Helper to convert text to level
            static levels stringToLevel(const std::string &levelStr);

            //! Helper to convert level to string
            static std::string levelToString(levels level);

            //! Queue message with log manager
            message *PostMessage();

            //! \name Message formatting functions
            //! Functions to format a log message with a variable number of arguments in a tye safe fashion using boost::format
            ///@{
            /**
             *  Format log message
             *  @param[in] fmtStr a printf style format string (see boost::format for details)
             *  @param[in] args   arguments to log
             */
            template <typename... Args>
            message *format(const std::string &fmtStr, const Args &... args)
            {
                m_format = boost::format(fmtStr);
                print(m_format, args...);

                return this;
            }

            ///@}

            /** @name Accessors
            *  Functions to access log message components
            */
            ///@{
            // *INDENT-OFF*
            std::string getMessage() const;                     //!< Get formatted log message
            uint32_t    getLine()     const { return m_line; }  //!< Get log message line number
            levels      getLevel()    const { return m_level; } //!< Get log message log level

            const boost::filesystem::path               &getFile()     const { return m_file; }     //!< Get log message file
            const std::string                           &getFunction() const { return m_function; } //!< Get log message function
            const std::string                           &getClass()    const { return m_class; }    //!< Get log message class
            const std::string                           &getModule()   const { return m_module; }   //!< Get log message module
            const std::chrono::system_clock::time_point &getDate()     const { return m_logTime; }  //!< Get log message timestamp
            const std::thread::id                       &getThreadID() const { return m_threadID; } //!< Get log message thread ID
            uint64_t                                     getHash()     const { return m_hash; }     //!< Get log message hash

            // *INDENT-ON*
            ///@}

            //! Get the raw boost format object, usable for streaming
            const boost::format &getMessageBody() const
            {
                return m_format;
            }

            //! \brief Get the raw boost format object for the header, usable for streaming
            //! These are the values that are available
            //!    - `%1%` Date
            //!    - `%2%` Time
            //!    - `%3%` Log level text
            //!    - `%4%` Thread ID
            //!    - `%5%` Filename
            //!    - `%6%` Filename with full path
            //!    - `%7%` Extended function name if defined, otherwise basic funtion name
            //!    - `%8%` Basic function name
            //!    - `%9%` Extended function name
            //!
            //! The default format string is `"%1% %2% %3% [%4%] %5% %7%"`
            const boost::format getMessageHeader(std::string formatStr);
            const boost::format getMessageHeaderConst(std::string formatStr) const;
            const boost::format getMessageHeader() const
            {
                // We should always pregenerate the default header
                if(m_headers.count(m_defaultFormatString))
                    return m_headers.find(m_defaultFormatString)->second;
                
                return getMessageHeaderConst(m_defaultFormatString);
            }


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
            std::string                             m_defaultFormatString;

            std::unordered_map<std::string, boost::format> m_headers;
    };

    //! print specialization for log level
    inline boost::format &print(boost::format &fmt, levels level)
    {
        return fmt % message::levelToString(level);
    }



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