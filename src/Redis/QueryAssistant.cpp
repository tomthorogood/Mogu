#include "QueryAssistant.h"

namespace Redis {

QueryBlueprint blueprintFactory ( 
        MoguSyntax cmd_type
        , Prefix prefix
        , MoguSyntax node_type)
{
    QueryBluePrint blueprint;
    switch(cmd_type)
    {
        case MoguSyntax::set:
        {
            switch(node_type)
            {
                case MoguSyntax::string: 
                    blueprint.appendLiteral("set",false);
                    break;
                case MoguSyntax::hash:
                    blueprint.appendLiteral("hset",false);
                    break;
                case MoguSyntax::list:
                    blueprint.appendLiteral("lset",false);
                    break;
            }//end switch node_type
        } break; //end case set

        case MoguSyntax::get:
        {
            switch(node_type)
            {
                case MoguSyntax::string:
                    blueprint.appendLiteral("get", false);
                    break;
                case MoguSyntax::hash:
                    blueprint.appendLiteral("hget",false);
                    break;
                case MoguSyntax::list:
                    blueprint.appendLiteral("lindex",false);
                    break;
            }//end switch node_type
        } break; //end case get
    }//end switch cmd_type

    blueprint.appendLiteral(prefixMap[prefix]+".");
    blueprint.appendString(false);

    switch(cmd_type)
    {
        case MoguSyntax::set:
        {
            switch(node_type)
            {
                case MoguSyntax::string:
                    blueprint.appendString();
                    break;
                case MoguSyntax::hash:
                    blueprint.appendString(false);
                    blueprint.appendString();
                    break;
                case MoguSyntax::list:
                    blueprint.appendInt(false);
                    blueprint.appendString();
            }//end switch node_type
        } break; //end case set

        case MoguSyntax::get:
        {
            switch(node_type)
            {
                case MoguSyntax::hash:
                    blueprint.appendString();
                    break;
                case MoguSyntax::list:
                    blueprint.appendInt();
                    break;
            }//end switch node_type
        }//end case get
    }//end switch cmd_type;
    return blueprint;
}


}//namespace Redis
