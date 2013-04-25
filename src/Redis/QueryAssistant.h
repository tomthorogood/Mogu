#ifndef QUERYASSISTANT_H_
#define QUERYASSISTANT_H_

#include <declarations.h>
#include "QueryBlueprint.h"
#include "QueryConstruct.h"

namespace Redis {

enum class CommandType
{
    set
    , get
    , append
};

enum class NodeType
{
    string
    , hash
    , list
};

const static std::unordered_map <std::string,NodeType> nodeTypeMap = {
    {"string" : NodeType::string},
    {"hash"   : NodeType::hash},
    {"list"   : NodeType::list}
};

QueryBlueprint blueprintFactory ( 
        CommandType cmd_type
        , Prefix prefix
        , NodeType node_type);

inline void addQuery(
        ContextQuery& db
        , CommandType cmd_type
        , Prefix prefix
        , NodeType node_type
        , CommandValue& cmd)
{
    QueryBlueprint blueprint = blueprintFactory(cmd_type,prefix,node_type);
    QueryConstruct construct(blueprint, cmd);
    construct.appendQuery(db);
}

}//namespace Redis

#endif
