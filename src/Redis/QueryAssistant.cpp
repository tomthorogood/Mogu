#include "QueryAssistant.h"
#include "QueryBlueprint.h"
namespace Redis {

QueryBlueprint blueprintFactory ( 
        MoguSyntax cmd_type
        , Prefix prefix
        , MoguSyntax node_type)
{
    QueryBlueprint blueprint;
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
                default: break;
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
                default: break;
            }//end switch node_type
        } break; //end case get

        default: break;
    }//end switch cmd_type

    std::string literal = prefixMap.at(prefix);
    literal += ".";
    blueprint.appendLiteral(literal);
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
                default: break;
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
                default: break;
            }//end switch node_type
        }//end case get
        default: break;
    }//end switch cmd_type;

    return blueprint;
}


}//namespace Redis
