#pragma once
#ifndef _LOG_CONFIG_H_
#define _LOG_CONFIG_H_

#include<unordered_map>
#include<boost/property_tree/ptree.hpp>
#include<boost/filesystem.hpp>

#include"log_message.h"

namespace LogXX
{
    class configuration
    {
        public:
            configuration(const boost::filesystem: path &path);

            bool logMessage(const std::shared_ptr<message> msg);

            boost::property_tree::ptree getBackends() const;
        private:
            std::unordered_map<uint64_t, bool> m_messageCache;
            boost::property_tree::ptree        m_configuration;
    };
}


#endif//_LOG_CONFIG_H_