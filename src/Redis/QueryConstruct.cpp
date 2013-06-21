#include "QueryConstruct.h"
#include "Query.h"

namespace Redis {


QueryConstruct::QueryConstruct(
        QueryBlueprint& blueprint_, CommandValue& command)
    : blueprint(blueprint_)
{
    //!\TODO FACTOR THIS
    size_t nargs = blueprint.nargs();
    node = command.getIdentifier();
    if (nargs == 2)
    {
        if (blueprint.specs == 2)
        {
            if (command.get(CommandFlags::ARG).getType() == ReadType::string_value)
                iarg1 = atoi(command.get(CommandFlags::ARG).getString().c_str());
            else iarg1 = command.get(CommandFlags::ARG).getInt();
        }
        else if (blueprint.specs == 3)
        {
            if (command.get(CommandFlags::ARG).getType() == ReadType::int_value)
                sarg1 = std::to_string(command.get(CommandFlags::ARG).getInt());
            else sarg1 = command.get(CommandFlags::ARG).getString();
        }
    }

    if (nargs == 3)
    {
        if (blueprint.specs == 5)
        {
            if (command.get(CommandFlags::ARG).getType() == ReadType::string_value)
                iarg1 = atoi(command.get(CommandFlags::ARG).getString().c_str());
            else iarg1 = command.get(CommandFlags::ARG).getInt();

            if (command.get(CommandFlags::VALUE).getType() == ReadType::int_value)
                sarg2 = std::to_string(command.get(CommandFlags::VALUE).getInt());
            else sarg2 = command.get(CommandFlags::VALUE).getString();
        }
        else if (blueprint.specs == 7)
        {
            if (command.get(CommandFlags::ARG).getType() == ReadType::int_value)
                sarg1 = std::to_string(command.get(CommandFlags::ARG).getInt());
            else sarg1 = command.get(CommandFlags::ARG).getString();

            if (command.get(CommandFlags::VALUE).getType() == ReadType::int_value)
                sarg2 = std::to_string(command.get(CommandFlags::VALUE).getInt());
            else sarg2 = command.get(CommandFlags::VALUE).getString();
        }
    }
}

void QueryConstruct::appendQuery(ContextQuery& db)
{
    switch(blueprint.specs)
    {
        case 1:
            db.appendQuery( blueprint.query.c_str(),node.c_str());
            break;

        case 2:
            db.appendQuery( blueprint.query.c_str(), node.c_str(), iarg1);
            break;
        case 3:
            db.appendQuery(blueprint.query.c_str(), node.c_str(),
                    sarg1.c_str());
            break;
        case 5:
            db.appendQuery(blueprint.query.c_str(), node.c_str(), iarg1,
                    sarg2.c_str());
            break;
        case 7:
            db.appendQuery(blueprint.query.c_str(), node.c_str(),
                    sarg1.c_str(), sarg2.c_str());
           break; 
    }
}

}//namespace Redis
