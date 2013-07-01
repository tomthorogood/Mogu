#ifndef NODEMERGER_H_
#define NODEMERGER_H_

#include <declarations.h>

namespace Redis {

class NodeMerger {
public:
    NodeMerger(std::string);

    inline void addPrefix(Prefix prefix, size_t priority)
    {
        std::pair <Prefix,size_t> pr =
            std::make_pair(prefix,priority);
        prefixes.push_back(pr);
    }

    std::map <std::string,std::string>  merge();
    void writeTemporary (std::map <std::string, std::string>);

private:
    std::vector <std::pair <Prefix, uint8_t>> prefixes;
    
    std::string node_name;
    
    MoguSyntax node_type        = MoguSyntax::__NONE__;

    // Sorts in descending order.
    inline bool sort_assist(
        std::pair <Prefix,size_t> alpha, std::pair <Prefix,size_t> beta)
    {
        return alpha.second > beta.second;
    }
};

}//namespace Redis
#endif
