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
        _trace0("Hello");
    }
    
    ~foo()
    {
        _trace0("bar");
    }
    
    void foobar()
    {
        _trace0("foobar");
    }
};

int main(void)
{
    std::cout << "Starting log test";
    
    auto logManager(std::make_shared<LogXX::manager>());
    logManager->addTarget(std::make_shared<LogXX::logCLog>());
    logManager->Run();

    foo f;

	_trace0("trace");
	_info0("info");
	_warn0("warning");
	_err0("error");
	_sev0("severe");
    
    f.foobar();
    
    std::this_thread::sleep_for(1s);
    
    f.foobar();
    
    auto func = []
    {
        _trace0("bar");
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
