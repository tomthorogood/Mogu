#ifndef NODEMERGER_H_
#define NODEMERGER_H_

#include <declarations.h>

namespace Redis {

class NodeMerger {
public:
    NodeMerger(const std::string&);

    inline void addPrefix(Prefix prefix, const std::string& name, size_t priority)
    {
        NodePairing pairing = std::make_pair(prefix, name);
        NodePriority priority_pr = std::make_pair(pairing, priority);
        prefixes.push_back(priority_pr);
    }

    std::map <std::string,std::string>  merge();
    void writeTemporary (std::map <std::string, std::string>);

private:
    typedef std::pair <Prefix, std::string> NodePairing;
    typedef std::pair <NodePairing, size_t> NodePriority;
    typedef std::vector <NodePriority> NodeVector;

    NodeVector prefixes;
    
    std::string node_name;
    
    SyntaxDef node_type        = MoguSyntax::__NONE__;
};

}//namespace Redis
#endif
