/**
 * @file   log_config.cpp
 * @author Gordon "Lee" Morgan (valkerie.fodder@gmail.com)
 * @copyright Copyright © Gordon "Lee" Morgan May 2016. This project is released under the [MIT License](license.md)
 * @date   May 2016
 * @brief  log configuration file
 * @details A class to read a configuration file for the logging manager
 */

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string.hpp>
#include "log_config.h"

namespace LogXX
{
    configuration::configuration(const boost::filesystem::path &path) : m_defaultLevel(LOG_NONE)
    {
        if(!path.empty())
        {
            auto extension(path.extension());

            if(extension.compare(".json") == 0)
            {
                boost::property_tree::read_json(path.generic_string(), m_configuration);
            }
            else
            {
                boost::property_tree::read_xml(path.generic_string(), m_configuration, boost::property_tree::xml_parser::no_comments | boost::property_tree::xml_parser::trim_whitespace);
                m_configuration = normalizePTree(m_configuration);
            }
        }
    }

    bool configuration::logMessage(const std::shared_ptr<message> msg)
    {
        uint hash(msg->getHash());
        if(m_messageCache.count(hash) == 0)
        {
            m_messageCache[hash] = getLevel(msg);
        }

        return msg->getLevel() >= m_messageCache[hash];
    }

    levels configuration::getLevel(const std::shared_ptr<message> msg)
    {
        levels level(m_defaultLevel);

        std::queue<boost::property_tree::ptree> treeQueue;
        treeQueue.push(m_configuration);

        while(!treeQueue.empty())
        {
            auto &node(treeQueue.front());
            if(node.count("level") > 0)
            {
                level = message::stringToLevel(node.get<std::string>("level"));
            }
            for(const auto &child : node)
            {
                bool enqueue(false);
                if(child.second.count("name") == 0)
                {
                    enqueue = true;
                }
                else
                {
                    std::string messageName;
                    if(boost::algorithm::iequals(child.first, "module"))
                    {
                        messageName = msg->getModule();
                    }
                    else if(boost::algorithm::iequals(child.first, "file"))
                    {
                        messageName = msg->getFile().generic_string();
                    }
                    else if(boost::algorithm::iequals(child.first, "function"))
                    {
                        messageName = msg->getFunction();
                    }
                    else if(boost::algorithm::iequals(child.first, "class"))
                    {
                        messageName = msg->getClass();
                    }

                    enqueue = boost::algorithm::iequals(messageName, child.second.get<std::string>("name"));

                }

                if(enqueue)
                {
                    treeQueue.push(child.second);
                }
            }

            treeQueue.pop();
        }

        return level;
    }

    boost::property_tree::ptree configuration::getBackends() const
    {
        boost::property_tree::ptree backends;
        std::queue<boost::property_tree::ptree> treeQueue;
        treeQueue.push(m_configuration);

        while(!treeQueue.empty())
        {
            auto &node(treeQueue.front());
            for(const auto &child : node)
            {
                if(boost::algorithm::iequals("backend", child.first))
                {
                    backends.push_back(child);
                }
                else
                {
                    treeQueue.push(child.second);
                }
            }

            treeQueue.pop();
        }


        return backends;

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
                normalizedTree.push_back({node.first, normalizePTree(node.second)});
            }

        }

        return normalizedTree;
    }
}
