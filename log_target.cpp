// LoggerPOC.cpp : Defines the entry point for the console application.
//
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
#include "log.h"
#include "logger.h"

using namespace std::chrono_literals;

logger::logger(boost::filesystem::path configFile)
{
    // Read logger configuration, blah blah blah...
}

void logger::Run()
{
    std::lock_guard<std::recursive_mutex> lock(m_logMutex);
    if(m_globalLogger.lock())
    {
        throw std::system_error(std::error_code(), "Only one instance of a logger allowed");
    }

    m_globalLogger = shared_from_this();


    // Launch thread with lambda and shared pointer to ensure that we don't get deleted out from under ourselves
    auto pThis(shared_from_this());
    std::thread logThread([pThis]
    {
        pThis->ThreadMain();
    });

    m_logThread.swap(logThread);
}

std::queue<std::shared_ptr<log>> logger::getMessages()
{
    std::lock_guard<std::recursive_mutex> lock(m_logMutex);

    std::queue<std::shared_ptr<log>> messages;
    messages.swap(m_messages);

    return messages;
}

void logger::ThreadMain()
{
    while(m_globalLogger.lock())
    {
        std::mutex waitMutex;
        std::unique_lock<std::mutex> lock(waitMutex);
        
        m_messagesWaiting.wait(lock);
        LogMessages();
    }
}

void logger::LogMessages()
{
    std::queue<std::shared_ptr<log>> messages(getMessages());
    
    while(!messages.empty())
    {
        auto msg(messages.front());
        messages.pop();
        for(auto logger: m_loggers)
        {
            logger->LogMessage(msg);
        }
    }
    
}

void logger::Shutdown()
{
    if(m_globalLogger.lock())
    {
        m_globalLogger.reset();
        m_messagesWaiting.notify_all();
        m_logThread.join();
        LogMessages(); // Dump any remaining messages
    }
}

void logger::pushMessage(std::shared_ptr<log> msg)
{
    std::lock_guard<std::recursive_mutex> lock(m_logMutex);
    m_messages.push(msg);
    m_messagesWaiting.notify_one();

}

void logger::logMessage(std::shared_ptr<log> msg)
{
    std::lock_guard<std::recursive_mutex> lock(m_logMutex);
    auto loggerPtr(m_globalLogger.lock());

    if(loggerPtr)
    {
        loggerPtr->pushMessage(msg);
    }
}

std::weak_ptr<logger> logger::m_globalLogger;
std::recursive_mutex  logger::m_logMutex;
