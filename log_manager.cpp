/**
 * @file   log_manager.cpp
 * @author Gordon "Lee" Morgan (valk.erie.fod.der+logxx@gmail.com)
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
#include "log_manager.h"
#include "log_message.h"

using namespace std::chrono_literals;

namespace LogXX
{

    manager::manager(boost::filesystem::path configFile) : m_config(configFile)
    {
    }

    void manager::Run()
    {
        std::lock_guard<std::recursive_mutex> lock(m_logMutex);

        if(m_globalmanager.lock())
        {
            throw std::system_error(std::error_code(), "Only one instance of a manager allowed");
        }

        m_globalmanager = shared_from_this();


        // Launch thread with lambda and shared pointer to ensure that we don't get deleted out from under ourselves
        auto pThis(shared_from_this());
        std::thread logThread([pThis]
        {
            pThis->ThreadMain();
        });

        m_logThread.swap(logThread);
    }

    std::queue<std::shared_ptr<message>> manager::getMessages()
    {
        std::lock_guard<std::recursive_mutex> lock(m_logMutex);

        std::queue<std::shared_ptr<message>> messages;
        messages.swap(m_messages);

        return messages;
    }

    void manager::ThreadMain()
    {
        while(m_globalmanager.lock())
        {
            std::mutex waitMutex;
            std::unique_lock<std::mutex> lock(waitMutex);

            m_messagesWaiting.wait(lock);
            LogMessages();
        }
    }

    void manager::LogMessages()
    {
        std::queue<std::shared_ptr<message>> messages(getMessages());

        while(!messages.empty())
        {
            auto msg(messages.front());
            messages.pop();

            for(auto manager : m_managers)
            {
                manager->LogMessage(msg);
            }
        }

    }

    void manager::Shutdown()
    {
        if(m_globalmanager.lock())
        {
            m_globalmanager.reset();
            m_messagesWaiting.notify_all();
            m_logThread.join();
            LogMessages(); // Dump any remaining messages
        }
    }

    void manager::pushMessage(std::shared_ptr<message> msg)
    {
        std::lock_guard<std::recursive_mutex> lock(m_logMutex);
        m_messages.push(msg);
        m_messagesWaiting.notify_one();

    }

    void manager::logMessage(std::shared_ptr<message> msg)
    {
        std::lock_guard<std::recursive_mutex> lock(m_logMutex);
        auto managerPtr(m_globalmanager.lock());

        if(managerPtr)
        {
            managerPtr->pushMessage(msg);
        }
    }

    std::weak_ptr<manager> manager::m_globalmanager;
    std::recursive_mutex  manager::m_logMutex;

}
