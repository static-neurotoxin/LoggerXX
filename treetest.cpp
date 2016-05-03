#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <queue>
#include <utility>

boost::property_tree::ptree NormalizePTree(const boost::property_tree::ptree &tree)
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

boost::property_tree::ptree OptimizeForXML(const boost::property_tree::ptree tree)
{
    boost::property_tree::ptree normalizedTree(tree.data());
    boost::property_tree::ptree xmlAttributes;

    for(auto node : tree)
    {
        if(node.second.empty())
        {
            xmlAttributes.push_back(node);
        }
        else
        {
            normalizedTree.push_back({node.first, OptimizeForXML(node.second)});
        }
    }

    if(!xmlAttributes.empty())
    {
        normalizedTree.push_front({"<xmlattr>", xmlAttributes});
    }

    return normalizedTree;
}


void DumpTree(const boost::property_tree::ptree &tree)
{
    std::queue<std::pair<std::string, boost::property_tree::ptree>> treeQueue;
    treeQueue.push({"", tree});

    while(!treeQueue.empty())
    {
        for(const auto &child : treeQueue.front().second)
        {
            std::cout << treeQueue.front().first << child.first << std::endl;
            treeQueue.push({treeQueue.front().first + child.first + '.', child.second});
        }

        treeQueue.pop();
    }
}

int main(void)
{
    auto testFile(boost::filesystem::path(__FILE__).parent_path() / "config.json");

    boost::property_tree::ptree tree;
    boost::property_tree::read_json(testFile.generic_string(), tree);
    DumpTree(tree);
    std::cout << "-----------" << std::endl;
    DumpTree(NormalizePTree(tree));
    std::cout << "-----------" << std::endl;
    DumpTree(OptimizeForXML(tree));
    auto testWriteFile(boost::filesystem::path(__FILE__).parent_path() / "optimize_config.xml");

    boost::property_tree::xml_writer_settings<std::string> settings(' ', 4);
    boost::property_tree::write_xml(testWriteFile.generic_string(),  OptimizeForXML(NormalizePTree(tree)), std::locale(), settings);

    boost::property_tree::write_json((boost::filesystem::path(__FILE__).parent_path() / "optimize_config.json").generic_string(), NormalizePTree(tree));
}