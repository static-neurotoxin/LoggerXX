/**
 * @file   log_message.h
 * @author Gordon "Lee" Morgan (valk.erie.fod.der+logxx@gmail.com)
 * @copyright Copyright © Gordon "Lee" Morgan May 2016. This project is released under the [MIT License](license.md)
 * @date   May 2016
 * @brief  log message formatting examples.
 * @details a couple of functions to demonstrate formatting variables
 *
 */

#pragma once
#ifndef _LOGFORMAT_H_
#define _LOGFORMAT_H_
#include <boost/format.hpp>

namespace LogXX
{
    //! typesafe print for any argument that overrides ostream << operator
    template <typename T>
    inline boost::format &print(boost::format &fmt, const T &t)
    {
        return fmt % t;
    }
    
    //! This print specialization will be called when the argument list is empty, terminating the recursion
    inline boost::format &print(boost::format &fmt)
    {
        return fmt;
    }
    
    //! Example print function to override type (bool)
    inline boost::format &print(boost::format &fmt, const bool b)
    {
        return fmt % (b ? "true" : "false");
    }
    
    //! This recursive template function pulls each argument off, left to right and formats it
    //! \param[in] first The argument to be formatted
    //! \param[in] rest The remaining unprocessed arguments
    template <typename First, typename... Rest>
    inline boost::format &print(boost::format &fmt, const First &first, const Rest &... rest)
    {
        return print(print(fmt, first), rest...);
    }
}

#endif//_LOG_H_