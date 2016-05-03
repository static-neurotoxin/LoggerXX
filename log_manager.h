// manager.h
//
// A (semi) lightweight manager for c++
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
    class manager : public std::enable_shared_from_this<manager>
    {
        public:
            manager(boost::filesystem::path configFile = boost::filesystem::path());

            void Run();
            std::queue<std::shared_ptr<message>> getMessages();
            void ThreadMain();
            void LogMessages();
            void Shutdown();
            void pushMessage(std::shared_ptr<message> msg);

            static void logMessage(std::shared_ptr<message> msg);

            inline void addTarget(std::shared_ptr<logTarget> target)
            {
                m_managers.push_back(target);
            }

            static std::weak_ptr<manager> m_globalmanager;
            static std::recursive_mutex  m_logMutex;

            std::thread m_logThread;
            std::queue<std::shared_ptr<message>> m_messages;
            std::condition_variable m_messagesWaiting;
            std::list<std::shared_ptr<logTarget>> m_managers;
    };
}
