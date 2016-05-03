#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "log_config.h"

namespace LogXX
{
        configuration::configuration(const boost::filesystem: path &path)
        {
            auto extension(path.extension());
            if(extension.compare(".json") == 0)
            {

            }
            else
            {

            }

        }

        bool configuration::logMessage(const std::shared_ptr<message> msg)
        {

        }

        boost::property_tree::ptree configuration::getBackends() const
        {

        }
}
