#include "NodeMerger.h"
#include "NodeEditor.h"
namespace Redis {

std::map <std::string, std::string> NodeMerger::merge()
{
    std::map <std::string,std::string> ret_map;

    // Since we're going in order of lowest to highest
    // priority (where the lowest integer has the highest 
    // priority), the map keys with a lower priority will
    // simply be overwritten by higher priority entries.
    for (auto pr : prefixes)
    {
        NodeEditor node(pr.first, node_name);
        node.readAll(ret_map);
    }

    return ret_map;

}

void NodeMerger::writeTemporary (std::map <std::string, std::string> source)
{
    NodeEditor node(Prefix::temp, node_name);
    node.writeAll(source);
}

}//namespace Redis
