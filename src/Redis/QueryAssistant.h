#ifndef QUERYASSISTANT_H_
#define QUERYASSISTANT_H_

#include <declarations.h>
#include "QueryBlueprint.h"
#include "QueryConstruct.h"

namespace Redis {

QueryBlueprint blueprintFactory ( 
        CommandType cmd_type
        , Prefix prefix
        , MoguSyntax node_type);

inline void addQuery(
        ContextQuery& db
        , MoguSyntax cmd_type
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
