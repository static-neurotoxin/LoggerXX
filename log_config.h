/**
 * @file   log_config.h
 * @author Gordon "Lee" Morgan (valk.erie.fod.der+logxx@gmail.com)
 * @copyright Copyright © Gordon "Lee" Morgan May 2016. This project is released under the [MIT License](license.md)
 * @date   May 2016
 * @brief  log configuration file
 * @details A class to read a configuration file for the logging manager
 */

#pragma once
#ifndef _LOG_CONFIG_H_
#define _LOG_CONFIG_H_

#include<unordered_map>
#include<boost/property_tree/ptree.hpp>
#include<boost/filesystem.hpp>

#include"log_message.h"

namespace LogXX
{
    //! Configuration file reader
    class configuration
    {
        public:
            //! Populate configuration with contents of xml or json file
            configuration(const boost::filesystem::path &path);

            //! check if debug message is enabled or not, cache results
            bool logMessage(const std::shared_ptr<message> msg);

            //! set default log level
            levels setDefaultLogLevel(levels level)
            {
                m_defaultLevel = level;
                return m_defaultLevel;
            }

            //! Return list of log back ends and their configuration
            boost::property_tree::ptree getBackends() const;
        private:
            //! Get the log level that is enabled for a given message
            levels getLevel(const std::shared_ptr<message> msg);

            //! Helper function to "promote" xml attributes to simple nodes
            boost::property_tree::ptree normalizePTree(const boost::property_tree::ptree &tree);

            std::unordered_map<uint64_t, levels> m_messageCache;
            boost::property_tree::ptree          m_configuration;
            levels                               m_defaultLevel;
    };
}


#endif//_LOG_CONFIG_H_