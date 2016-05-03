#include <iostream>
#include <cstdlib>
#include <chrono>
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
