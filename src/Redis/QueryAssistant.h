#ifndef QUERYASSISTANT_H_
#define QUERYASSISTANT_H_

#include <declarations.h>
#include "QueryBlueprint.h"
#include "QueryConstruct.h"
#include <Events/CommandValue.h>
namespace Redis {

QueryBlueprint blueprintFactory ( 
        MoguSyntax cmd_type
        , Prefix prefix
        , MoguSyntax node_type);

inline void addQuery(
        ContextQuery& db
        , MoguSyntax cmd_type
        , Prefix prefix
        , MoguSyntax node_type
        , CommandValue& cmd)
{
    QueryBlueprint blueprint = blueprintFactory(cmd_type,prefix,node_type);
    QueryConstruct construct(blueprint, cmd);
    construct.appendQuery(db);
}

}//namespace Redis

#endif
