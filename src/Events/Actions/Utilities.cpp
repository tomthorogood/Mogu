#include "../Actions.h"
#include "Includes.h"
namespace Actions {

MoguSyntax getPolicyNodeType(const std::string& identifier)
{
    Redis::ContextQuery policydb(Prefix::policies);
    policydb.appendQuery("hget policies.%s %d", identifier.c_str(),
            MoguSyntax::type);
    return policydb.yieldResponse<MoguSyntax>();
}

void policyQuery(
        MoguSyntax queryType
        , Redis::ContextQuery& db
        , Prefix prefix, CommandValue& v
        , bool init)
{
    mApp;
    std::string group = std::to_string(app->getGroup());

    MoguSyntax node_type = getPolicyNodeType(v.getIdentifier());
    if (!init) goto lbl_append_query;
    switch(prefix)
    {
        case Prefix::user:
            v.set(CommandFlags::IDENTIFIER, 
                    app->getUser()
                    + std::string(".")
                    + v.getIdentifier());
            break;
        case Prefix::group:
            v.set(CommandFlags::IDENTIFIER,  group + "." + v.getIdentifier());
            break;
        default:return;
    }
lbl_append_query:
    Redis::addQuery(
        db
        , queryType
        , prefix
        , node_type
        , v);
}

}//namespace Actions
