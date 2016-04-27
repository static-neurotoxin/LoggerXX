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

#include"date.h"

class logTarget
{
    public:
        virtual void LogMessage(std::shared_ptr<log> msg) = 0;
};

class logCLog : public logTarget
{
    public:
        void LogMessage(std::shared_ptr<log> msg) override
        {
            std::clog << msg << std::endl;
        }

};

class logFile : public logTarget
{
public:
    logFile(const boost::filesystem::path logFile) : m_file(logFile)
    {
        
    }

    void LogMessage(std::shared_ptr<log> msg) override
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
class logDebugConsole : public logTarget
{
public:
    void LogMessage(std::shared_ptr<log> msg) override
    {
        std::stringstream s;
        s << msg;

        //TODO: Unicode debug output is tricky; This logTarget will need to be more complex
        ::OutputDebugStringA(s);
    }
};
#endif

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
