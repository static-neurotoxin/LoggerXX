// logger.h
//
// A (semi) lightweight logger for c++
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

class logger : public std::enable_shared_from_this<logger>
{
    public:
    logger(boost::filesystem::path configFile = boost::filesystem::path());
    
    void Run();
    std::queue<std::shared_ptr<log>> getMessages();
    void ThreadMain();
    void LogMessages();
    void Shutdown();
    void pushMessage(std::shared_ptr<log> msg);

    static void logMessage(std::shared_ptr<log> msg);

    inline void addTarget(std::shared_ptr<logTarget> target)
    {
        m_loggers.push_back(target);
    }

    static std::weak_ptr<logger> m_globalLogger;
    static std::recursive_mutex  m_logMutex;

    std::thread m_logThread;
    std::queue<std::shared_ptr<log>> m_messages;
    std::condition_variable m_messagesWaiting;
    std::list<std::shared_ptr<logTarget>> m_loggers;
};
