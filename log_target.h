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
#include "log_message.h"

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
