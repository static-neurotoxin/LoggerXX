#include <iostream>
#include <cstdlib>
#include <chrono>
#include "log_message.h"
#include "log_manager.h"

using namespace std::chrono_literals;

int main(void)
{
    std::cout << "Starting log test";

    auto logManager(std::make_shared<logger>());
    logManager->addTarget(std::make_shared<logCLog>());
    logManager->Run();

	_trace0("trace");
	_info0("info");
	_warn0("warning");
	_err0("error");
	_sev0("severe");
	_fat0("fatal");
    
    std::this_thread::sleep_for(1s);

	_trace("trace: %d %s", std::rand(), "foo bar baz");
	_info("info: %d %s", std::rand(), "foo bar baz");
	_warn("warn: %d %s", std::rand(), "foo bar baz");
	_err("err: %d %s", std::rand(), "foo bar baz");
	_sev("sev: %d %s", std::rand(), "foo bar baz");
	_fat("fat: %d %s", std::rand(), "foo bar baz");


    logManager->Shutdown();
    logManager.reset();
}
