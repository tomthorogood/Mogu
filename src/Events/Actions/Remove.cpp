#include "../Actions.h"
#include "Includes.h"

namespace Actions {

void remove (Moldable& broadcaster, CommandValue& v)
{
    if (v.get(CommandFlags::VALUE).getType() == ReadType::int_value)
        v.get(CommandFlags::VALUE).setString(std::to_string(v.get(CommandFlags::VALUE).getInt()));
    
    switch(v.getObject())
    {
        case MoguSyntax::own:{
            MoguSyntax attribute = (MoguSyntax) v.get(CommandFlags::ARG);
            switch(attribute)
            {
                // remove "foo" from own css
                // remove own css
                case MoguSyntax::style:
                    if (v.get(CommandFlags::VALUE).getString() == EMPTY)
                    {
                        broadcaster.setStyleClass("");
                        return;
                    }
                    broadcaster.removeStyleClass(v.get(CommandFlags::VALUE).getString());
                    break;
                case MoguSyntax::text:
                    broadcaster.setMoldableValue("");
                    break;
                case MoguSyntax::index:{
                    NodeValue tmp(0);
                    broadcaster.setAttribute(attribute, tmp);}
                    break;
                default: return;
            }
            break;}
                    
        case MoguSyntax::widget:{
            mApp;
            MoguSyntax attribute = (MoguSyntax) v.get(CommandFlags::ARG);
            Moldable* widget = app->registeredWidget(v.getIdentifier());
            switch(attribute)
            {
                case MoguSyntax::style:
                    if (v.get(CommandFlags::VALUE).getString() == EMPTY)
                    {
                        broadcaster.setStyleClass("");
                        return;
                    }
                    widget->removeStyleClass(v.get(CommandFlags::VALUE).getString());
                    break;
                case MoguSyntax::text:
                    widget->setMoldableValue("");
                    break;
                case MoguSyntax::index:{
                    NodeValue tmp(0);
                    widget->setAttribute(attribute, tmp);
                    break;
                }
                case MoguSyntax::widget:{//remove widget foo from widget bar
                    Moldable* child = app->registeredWidget(
                            v.get(CommandFlags::VALUE).getString());
                    widget->removeChild(child);
                    break;}
                default:return;
            }
            break;}
        /* remove "an item" from user someList"
         * remove user someString
         * remove user someHash someField
         */
        case MoguSyntax::user:{
            mApp;
            std::string node =
                app->getUser()
                + std::string(".")
                + v.getIdentifier();
            Redis::ContextQuery policydb(Prefix::policies);
            Redis::ContextQuery db(Prefix::user);

            MoguSyntax node_type = getPolicyNodeType(v.getIdentifier());

            bool del_node = false;
            switch(node_type)
            {
                case MoguSyntax::list:
                    if (v.get(CommandFlags::VALUE).getString() != EMPTY)
                    {
                        db.appendQuery("ldel user.%s 1 %s"
                                , node.c_str()
                                , v.get(CommandFlags::VALUE).getString().c_str());
                        db.execute();
                        return;
                    }
                    else del_node = true;
                    break;
                case MoguSyntax::hash:
                    if (v.get(CommandFlags::ARG).getString() != EMPTY)
                    {
                        db.appendQuery("hdel user.%s %s"
                                , node.c_str()
                                , v.get(CommandFlags::ARG).getString().c_str());
                        db.execute();
                        return;
                    }
                    else del_node = true;
                    break;
                case MoguSyntax::string:
                    del_node = true;
                    break;                    
                default:return;
            }

            if (del_node)
            {
                db.appendQuery("del user.%s", node.c_str());
                return;
            }
            break;}

        case MoguSyntax::group:{
            mApp;
            std::string group = std::to_string (app->getGroup());
            std::string node = group + "." + v.getIdentifier();
            Redis::ContextQuery db(Prefix::group);

            MoguSyntax node_type = getPolicyNodeType(v.getIdentifier());
            
            bool del_node = false;

            switch(node_type)
            {
                case MoguSyntax::list:
                    if (v.get(CommandFlags::VALUE).getString() != EMPTY)
                    {
                        db.appendQuery("ldel group.%s 1 %s"
                                , node.c_str()
                                , v.get(CommandFlags::VALUE).getString().c_str());
                        db.execute();
                        return;
                    }
                    else del_node = true;
                    break;
                case MoguSyntax::hash:
                    if (v.get(CommandFlags::ARG).getString() != EMPTY)
                    {
                        db.appendQuery("hdel group.%s %s"
                                , node.c_str()
                                , v.get(CommandFlags::ARG).getString().c_str());
                        db.execute();
                        return;
                    }
                    else del_node = true;
                    break;
                case MoguSyntax::string:
                    del_node = true;
                    break;                    

                default: return;
            }

            if (del_node)
            {
                db.appendQuery("del group.%s", node.c_str());
                db.execute();
                return;
            }
            break;}

        // remove data someList someValue
        // remove data someHash someField
        // remove data someNode
        case MoguSyntax::data:{
            Redis::ContextQuery db(Prefix::data);

            db.appendQuery("type data.%s", v.getIdentifier().c_str());
            MoguSyntax node_type = db.yieldResponse<MoguSyntax>();
            bool del_node = false;
            switch(node_type)
            {
                case MoguSyntax::list:
                    if (v.get(CommandFlags::ARG).getString() != EMPTY) 
                    {
                        db.appendQuery(
                            "ldel data.%s 1 %s"
                            , v.getIdentifier().c_str()
                            , v.get(CommandFlags::ARG).getString().c_str());
                        db.execute();
                        return;
                    } else del_node = true;
                    break;
                case MoguSyntax::hash:
                    if (v.get(CommandFlags::ARG).getString() != EMPTY)
                    {
                        db.appendQuery(
                            "hdel data.%s %s"
                            , v.getIdentifier().c_str()
                            , v.get(CommandFlags::ARG).getString().c_str());
                        db.execute();
                        return;
                    } else del_node = true;
                    break;
                case MoguSyntax::string:
                    del_node = true;
                    break;
                default: return;
            }
            if (del_node)
            {
                db.appendQuery("del data.%s", v.getIdentifier().c_str());
                db.execute();
            }

            break;}

        // remove slot foo
        case MoguSyntax::slot:{
            mApp;
            app->slotManager().retrieveSlot(v.getIdentifier());
            break;}
        default: return;
        }
}

}//namespace Actions
