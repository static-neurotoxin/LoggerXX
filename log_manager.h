/**
 * @file   log_manager.h
 * @author Gordon "Lee" Morgan (valkerie.fodder@gmail.com)
 * @copyright Copyright © Gordon "Lee" Morgan May 2016. This project is released under the [MIT License](license.md)
 * @date   May 2016
 * @brief  Global log message manager.
 * @details A class to manage and push incoming messages to enabled message back ends in a thread safe fashion
 *
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
#include "log_target.h"
namespace LogXX
{
    //! Main thread manager
    class manager : public std::enable_shared_from_this<manager>
    {
        public:
            manager(boost::filesystem::path configFile = boost::filesystem::path());

            void Run();                                                 //!< Start log manager thread
            void Shutdown();                                            //!< End log manager thread
            static void logMessage(std::shared_ptr<message> msg);       //!< Enque a single log message, if manager is running

            //! Add a log back end
            inline void addTarget(std::shared_ptr<logTarget> target)
            {
                m_managers.push_back(target);
            }

        private:
            std::queue<std::shared_ptr<message>> getMessages();     //!< Get messages to be logged
            void pushMessage(std::shared_ptr<message> msg);         //!< Enqueue a single log message
            void ThreadMain();                                      //!< Main thread for logging
            void LogMessages();                                     //!< Send all currently queued messages to backends

            static std::weak_ptr<manager> m_globalmanager;
            static std::recursive_mutex  m_logMutex;

            std::thread m_logThread;
            std::queue<std::shared_ptr<message>> m_messages;
            std::condition_variable m_messagesWaiting;
            std::list<std::shared_ptr<logTarget>> m_managers;
    };
}
