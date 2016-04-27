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

#include "log_target.h"