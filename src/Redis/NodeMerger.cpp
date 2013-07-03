#include "NodeMerger.h"
#include "NodeEditor.h"
#include <algorithm>


namespace {

typedef std::pair <Prefix, std::string> NodePairing;
typedef std::pair <NodePairing, size_t> NodePriority;
typedef std::vector <NodePriority> NodeVector;

// Sorts in descending order.
bool sort_assist(
    NodePriority alpha, NodePriority beta)
{
    return alpha.second > beta.second;
}
}//anonymous namespace

namespace Redis {

NodeMerger::NodeMerger (const std::string& __node) :
    node_name(__node), node_type(MoguSyntax::__NONE__)
{
}



std::map <std::string, std::string> NodeMerger::merge()
{
    std::map <std::string,std::string> ret_map;
    std::sort (prefixes.begin(), prefixes.end(), sort_assist);

    // Since we're going in order of lowest to highest
    // priority (where the lowest integer has the highest 
    // priority), the map keys with a lower priority will
    // simply be overwritten by higher priority entries.
    for (auto pr : prefixes)
    {
        NodeEditor node(pr.first.first, pr.first.second);
        node.readAll(ret_map);
    }

    return ret_map;

}

void NodeMerger::writeTemporary (std::map <std::string, std::string> source)
{
    NodeEditor node(Prefix::temp, node_name, MoguSyntax::hash);
    node.writeAll(source);
}

}//namespace Redis
