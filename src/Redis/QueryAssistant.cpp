#include "QueryAssistant.h"

namespace Redis {

QueryBluePrint bluePrintFactory ( 
        CommandType cmd_type
        , Prefix prefix
        , NodeType node_type)
{
    QueryBluePrint blueprint;
    switch(cmd_type)
    {
        case CommandType::set
        {
            switch(node_type)
            {
                case NodeType::string: 
                    blueprint.appendLiteral("set",false);
                    break;
                case NodeType::hash:
                    blueprint.appendLiteral("hset",false);
                    break;
                case NodeType::list:
                    blueprint.appendLiteral("lset",false);
                    break;
            }//end switch node_type
        } break; //end case set

        case CommandType::get
        {
            switch(node_type)
            {
                case NodeType::string:
                    blueprint.appendLiteral("get", false);
                    break;
                case NodeType::hash:
                    blueprint.appendLiteral("hget",false);
                    break;
                case NodeType::list:
                    blueprint.appendLiteral("lindex",false);
                    break;
            }//end switch node_type
        } break; //end case get
    }//end switch cmd_type

    blueprint.appendLiteral(prefixMap[prefix]+".");
    blueprint.appendString(false);

    switch(cmd_type)
    {
        case CommandType::set
        {
            switch(node_type)
            {
                case NodeType::string:
                    blueprint.appendString();
                    break;
                case NodeType::hash:
                    blueprint.appendString(false);
                    blueprint.appendString();
                    break;
                case NodeType::list:
                    blueprint.appendInt(false);
                    blueprint.appendString();
            }//end switch node_type
        } break; //end case set

        case CommandType::get
        {
            switch(node_type)
            {
                case NodeType::hash:
                    blueprint.appendString();
                    break;
                case NodeType::list:
                    blueprint.appendInt();
                    break;
            }//end switch node_type
        }//end case get
    }//end switch cmd_type;
    return blueprint;
}


}//namespace Redis
