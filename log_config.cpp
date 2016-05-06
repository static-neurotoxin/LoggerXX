/**
 * @file   log_config.cpp
 * @author Gordon "Lee" Morgan (valkerie.fodder@gmail.com)
 * @copyright Copyright © Gordon "Lee" Morgan May 2016. This project is released under the MIT License
 * @date   May 2016
 * @brief  log configuration file
 * @details A class to read a configuration file for the logging manager
 */

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "log_config.h"

namespace LogXX
{
    configuration::configuration(const boost::filesystem::path &path)
    {
        auto extension(path.extension());

        if(extension.compare(".json") == 0)
        {
            boost::property_tree::read_json(path.generic_string(), m_configuration);
        }
        else
        {
            boost::property_tree::read_xml(path.generic_string(), m_configuration, boost::property_tree::xml_parser::no_comments | boost::property_tree::xml_parser::trim_whitespace);

        }

    }

    bool configuration::logMessage(const std::shared_ptr<message> msg)
    {
        uint hash(msg->getHash());
        if(m_messageCache.count(hash) == 0)
        {
            m_messageCache[hash] = msg->getLevel() >= getLevel(msg);
        }

        return m_messageCache[hash];
    }

    levels configuration::getLevel(const std::shared_ptr<message> msg)
    {

    }

    boost::property_tree::ptree configuration::getBackends() const
    {

    }


    boost::property_tree::ptree configuration::normalizePTree(const boost::property_tree::ptree &tree)
    {
        boost::property_tree::ptree normalizedTree(tree.data());

        for(auto node : tree)
        {
            if(node.first == "<xmlattr>")
            {
                for(auto attribute : node.second)
                {
                    normalizedTree.push_back(attribute);
                }
            }
            else
            {
                normalizedTree.push_back({node.first, NormalizePTree(node.second)});
            }

        }

        return normalizedTree;
    }
}
