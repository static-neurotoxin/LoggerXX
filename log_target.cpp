/**
 * @file   log_target.cpp
 * @author Gordon "Lee" Morgan (valkerie.fodder@gmail.com)
 * @copyright Copyright © Gordon "Lee" Morgan May 2016. This project is released under the MIT License
 * @date   May 2016
 * @brief  log back end interface and provided log back ends.
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

#include "log_target.h"