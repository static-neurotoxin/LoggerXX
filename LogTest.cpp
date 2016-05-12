#include <iostream>
#include <cstdlib>
#include <chrono>

#include <boost/format.hpp>
enum foobar {FOO_FOO, FOO_BAR, FOO_BAZ};

namespace LogXX
{
    boost::format &print(boost::format &fmt, const foobar fb)
    {
        switch(fb)
        {
            case FOO_FOO:
                return fmt % "foo";
                
            case FOO_BAR:
                return fmt % "bar";
                
            case FOO_BAZ:
                return fmt % "baz";
        }
        
        return fmt % "unknown enum";
    }
}

#include "log_message.h"
#include "log_manager.h"

using namespace std::chrono_literals;
class foo
{
    public:
        foo()
        {
            _trace("Hello");
        }

        ~foo()
        {
            _trace("bar");
        }

        void foobar()
        {
            _trace("foobar");
        }
};

int main(void)
{
    std::cout << "Starting log test";

    auto logManager(std::make_shared<LogXX::manager>());
    logManager->addTarget(std::make_shared<LogXX::logCLog>());
    logManager->Run();

    foo f;

    _trace("trace");
    _info("info");
    _warn("warning");
    _err("error");
    _sev("severe");
    
    _trace("bool test %1%", true);
    _trace("enum test 1 %1%", LogXX::LOG_WARNING);
    _trace("enum test 2 %1% %2% %3%", FOO_FOO, FOO_BAR, FOO_BAZ);

    f.foobar();

    std::this_thread::sleep_for(1s);

    f.foobar();

    auto func = []
    {
        _trace("bar");
    };

    func();

    _trace("trace: %d %s", std::rand(), "foo bar baz");
    _info("info: %d %s", std::rand(), "foo bar baz");
    _warn("warn: %d %s", std::rand(), "foo bar baz");
    _err("err: %d %s", std::rand(), "foo bar baz");
    _sev("sev: %d %s", std::rand(), "foo bar baz");


    logManager->Shutdown();
    logManager.reset();
}
