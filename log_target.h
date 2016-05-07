/**
 * @file   log_target.h
 * @author Gordon "Lee" Morgan (valkerie.fodder@gmail.com)
 * @copyright Copyright © Gordon "Lee" Morgan May 2016. This project is released under the MIT License
 * @date   May 2016
 * @brief  log back end interface and provided log back ends.
 */

#include <iostream>
#include <string>
#include <sstream>
#include <cstdint>
#include <chrono>
#include <memory>
#include <mutex>
#include <system_error>
#include <queue>
#include <condition_variable>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include "date/date.h"
#include "log_message.h"

namespace LogXX
{
    //! Interface for log back ends
    class logTarget
    {
        public:
            virtual void LogMessage(std::shared_ptr<message> msg) = 0; //!< Log message to back end
    };

    //! Log back end that sends log messages to `std::clog` stream
    class logCLog : public logTarget
    {
        public:
            //! Log message to `std::clog`
            void LogMessage(std::shared_ptr<message> msg) override
            {
                std::clog << msg << std::endl;
            }

    };

    //! Log back end that sends log messages to a file
    class logFile : public logTarget
    {
        public:
            logFile(const boost::filesystem::path logFile) : m_file(logFile)
            {

            }

            //! Log message to file
            void LogMessage(std::shared_ptr<message> msg) override
            {
                if(m_file.is_open())
                {
                    m_file << msg << std::endl;
                }
            }

        private:
            boost::filesystem::ofstream m_file;
    };

#ifdef _WIN32
    //! Log back end that sends log messages to `::OutputDebugString()`
    class logDebugConsole : public logTarget
    {
        public:
            // Log message to ::OutputDebugStringA
            void LogMessage(std::shared_ptr<log> msg) override
            {
                std::stringstream s;
                s << msg;

                //TODO: Unicode debug output is tricky; This logTarget will need to be more complex
                ::OutputDebugStringA(s);
            }
    };
#endif
}