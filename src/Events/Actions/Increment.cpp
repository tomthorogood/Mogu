#include "../Actions.h"
#include "Includes.h"

namespace Actions {

void increment (Moldable& broadcaster, CommandValue& v)
{
    int value = 
        (v.getFlags() & (uint8_t) CommandFlags::VALUE) ?
        v.get(CommandFlags::VALUE).getInt() : 1;
    NodeValue final;
    switch(v.getObject())
    {
        case MoguSyntax::own:{
            NodeValue current;
            broadcaster.getAttribute( (MoguSyntax) v.get(CommandFlags::ARG), current);
            if (current.getType() == ReadType::string_value)
                current.setInt(atoi(current.getString().c_str()));
            current.setInt(current.getInt()+value);
            broadcaster.setAttribute( 
                    (MoguSyntax) v.get(CommandFlags::ARG), current);
            break;}

        case MoguSyntax::user:{
            Redis::ContextQuery db(Prefix::user);
            policyQuery(
                MoguSyntax::get
                , db
                , Prefix::user
                , v);
            policyQuery(
                MoguSyntax::set
                , db
                , Prefix::user
                , v);

            final.setInt(
                final.getInt() + atoi(db.yieldResponse<std::string>().c_str())
            );
            db.execute();
            break;}

        case MoguSyntax::group:{
            Redis::ContextQuery db(Prefix::group);
            policyQuery(
                MoguSyntax::get
                , db
                , Prefix::group
                , v);
            policyQuery(
                MoguSyntax::set
                , db
                , Prefix::group
                , v);

            final.setInt(
                final.getInt() + atoi(db.yieldResponse<std::string>().c_str())
            );
            db.execute();
            break;}

        case MoguSyntax::data:{
            Redis::ContextQuery db(Prefix::data);
            final.setInt(value);
            db.appendQuery("type data.%s", v.getIdentifier().c_str());
            MoguSyntax node_type = db.yieldResponse <MoguSyntax>();
            v.set(CommandFlags::VALUE, final);
            Redis::addQuery(
                db
                , MoguSyntax::get
                , Prefix::data
                , node_type
                , v);
            Redis::addQuery(
                db
                , MoguSyntax::set
                , Prefix::data
                , node_type
                , v);
            final.setInt(
                final.getInt() + atoi(db.yieldResponse<std::string>().c_str())
            );
            db.execute();

            break;}
        case MoguSyntax::slot:{
            mApp;
            final.setInt(
                app->slotManager().retrieveSlot(v.getIdentifier()).getInt());
            final.setInt(final.getInt()+value);
            app->slotManager().setSlot(
                    v.getIdentifier(), final);
            break;}

        case MoguSyntax::widget:{
            mApp;
            Moldable* widget = app->registeredWidget(v.getIdentifier());
            NodeValue current;
            widget->getAttribute( (MoguSyntax) v.get(CommandFlags::ARG), current);
            if (current.getType() == ReadType::string_value)
                current.setInt(atoi(current.getString().c_str()));
            current.setInt(current.getInt()+value);
            widget->setAttribute( (MoguSyntax) v.get(CommandFlags::ARG), current );
            break;}
        default: return;
    }
} 

}//namespace Actions
