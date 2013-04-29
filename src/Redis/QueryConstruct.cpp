#include "QueryConstruct.h"
#include "ContextQuery.h"
#include "Query.h"

namespace Redis {


QueryConstruct::QueryConstruct(
        QueryBlueprint& blueprint_, const CommandValue& command)
    : blueprint(blueprint_)
{
    //!\TODO FACTOR THIS
    size_t nargs = blueprint.nargs();
    node = command.getIdentifier();
    if (nargs == 2)
    {
        if (blueprint.specs == 2)
        {
            if (command.getArg().getType() == string_type)
                iarg1 = atoi(command.getArg().getString().c_str());
            else iarg1 = command.getArg().getInt();
        }
        else if (blueprint.specs == 3)
        {
            if (command.getArg.getType() == integer_type)
                sarg1 = std::to_string(command.getArg().getInt());
            else sarg1 = command.getArg().getString();
        }
    }

    if (nargs == 3)
    {
        if (blueprint.specs == 5)
        {
            if (command.getArg().getType() == string_type)
                iarg1 = atoi(command.getArg().getString().c_str());
            else iarg1 = command.getArg().getInt();

            if (command.getValue().getType() == integer_type)
                sarg2 = std::to_string(command.getValue().getInt());
            else sarg2 = command.getValue().getString();
        }
        else if (blueprint.specs == 7)
        {
            if (command.getArg().getType() == integer_type)
                sarg1 = std::to_string(command.getArg().getInt());
            else sarg1 = command.getArg().getString();

            if (command.getValue().getType() == integer_type)
                sarg2 = std::to_string(command.getValue().getInt());
            else sarg2 = command.getValue().getString();
        }
    }
}

void QueryConstruct::appendQuery(ContextQuery& db)
{
    switch(blueprint.specs)
    {
        case 1:
            CreateQuery(db, new Query(blueprint.query.c_str(),node.c_str()));
            break;

        case 2:
            CreateQuery(db, 
                    new Query(blueprint.query.c_str(), node.c_str(), iarg1));
            break;
        case 3:
            CreateQuery(db,
                    new Query(blueprint.query.c_str(), node.c_str(), 
                        sarg1.c_str()));
            break;
        case 5:
            CreateQuery(db,
                    new Query(blueprint.query.c_str(), node.c_str(),
                        iarg1, sarg2.c_str()));
            break;
        case 7:
            CreateQuery(db,
                    new Query(blueprint.query.c_str(), node.c_str(),
                       sarg1.c_str(), sarg2.c_str()));
           break; 
    }
}

}//namespace Redis
